#include "HLSLDesc.h"

#if !VG_ENABLE_INLINE
#include "HLSLDesc.inl"
#endif

namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    HLSLDesc::~HLSLDesc()
    {
        for (uint i = 0; i < countof(m_flagDescs); ++i)
            m_flagDescs[i].clear();

        for (uint i = 0; i < core::enumCount<ShaderStage>(); ++i)
            m_entryPoint[i].clear();

        m_techniques.clear();

        for (auto & pair : m_variants)
            VG_SAFE_RELEASE(pair.second);
    }

    //--------------------------------------------------------------------------------------
    void HLSLDesc::setFile(const core::string & _file)
    {
        m_file = _file;
    }

    //--------------------------------------------------------------------------------------
    const core::string & HLSLDesc::getFile() const
    {
        return m_file;
    }

    //--------------------------------------------------------------------------------------
    ShaderKey::VS HLSLDesc::declVS(const core::string & _vsEntryPoint)
    {
        return (ShaderKey::VS)declShader(ShaderStage::Vertex, _vsEntryPoint);
    }

    //--------------------------------------------------------------------------------------
    ShaderKey::HS HLSLDesc::declHS(const core::string & _hsEntryPoint)
    {
        return (ShaderKey::HS)declShader(ShaderStage::Hull, _hsEntryPoint);
    }

    //--------------------------------------------------------------------------------------
    ShaderKey::DS HLSLDesc::declDS(const core::string & _dsEntryPoint)
    {
        return (ShaderKey::DS)declShader(ShaderStage::Domain, _dsEntryPoint);
    }

    //--------------------------------------------------------------------------------------
    ShaderKey::GS HLSLDesc::declGS(const core::string & _gsEntryPoint)
    {
        return (ShaderKey::GS)declShader(ShaderStage::Geometry, _gsEntryPoint);
    }

    //--------------------------------------------------------------------------------------
    ShaderKey::PS HLSLDesc::declPS(const core::string & _psEntryPoint)
    {
        return (ShaderKey::PS)declShader(ShaderStage::Pixel, _psEntryPoint);
    }

    //--------------------------------------------------------------------------------------
    ComputeShaderKey::CS HLSLDesc::declCS(const core::string & _csEntryPoint)
    {
        return (ComputeShaderKey::CS)declShader(ShaderStage::Compute, _csEntryPoint);
    }

    //--------------------------------------------------------------------------------------
    Shader * HLSLDesc::getVS(API _api, ShaderKey::VS _vs, ShaderKey::Flags _flags)
    {
        return getShader(_api, ShaderStage::Vertex, _vs, _flags);
    }    

    //--------------------------------------------------------------------------------------
    Shader * HLSLDesc::getHS(API _api, ShaderKey::HS _hs, ShaderKey::Flags _flags)
    {
        return getShader(_api, ShaderStage::Hull, _hs, _flags);
    }

    //--------------------------------------------------------------------------------------
    Shader * HLSLDesc::getDS(API _api, ShaderKey::DS _ds, ShaderKey::Flags _flags)
    {
        return getShader(_api, ShaderStage::Domain, _ds, _flags);
    }

    //--------------------------------------------------------------------------------------
    Shader * HLSLDesc::getGS(API _api, ShaderKey::GS _gs, ShaderKey::Flags _flags)
    {
        return getShader(_api, ShaderStage::Geometry, _gs, _flags);
    }

    //--------------------------------------------------------------------------------------
    Shader * HLSLDesc::getPS(API _api, ShaderKey::PS _ps, ShaderKey::Flags _flags)
    {
        return getShader(_api, ShaderStage::Pixel, _ps, _flags);
    }

    //--------------------------------------------------------------------------------------
    ShaderKey::EntryPoint HLSLDesc::declShader(ShaderStage _stage, const core::string & _entryPoint)
    {
        auto & stageEntryPoints = m_entryPoint[asInteger(_stage)];
        
        for (uint i = 0; i < stageEntryPoints.size(); ++i)
        {
            const auto & entryPoint = stageEntryPoints[i];
            if (entryPoint == _entryPoint)
                return (ShaderKey::EntryPoint)i; // Already exists
        }

        stageEntryPoints.push_back(_entryPoint);
        auto index = (ShaderKey::EntryPoint)(stageEntryPoints.size() - 1);

        VG_INFO("[Shader] \"%s\": Add %s Shader \"%s\" (%u)", getFile().c_str(), asString(_stage).c_str(), _entryPoint.c_str(), index);

        return index;
    }

    //--------------------------------------------------------------------------------------
    Shader * HLSLDesc::getShader(API _api, ShaderStage _stage, ShaderKey::EntryPoint _index, ShaderKey::Flags _flags)
    {
        auto & entryPoint = m_entryPoint[asInteger(_stage)][_index]; 
        
        VariantKey key = computeVariantKey(_stage, _index, _flags);

        auto it = m_variants.find(key);

        if (m_variants.end() != it)
        {
            if (nullptr != it->second)
                return it->second;
        }

        auto * sm = ShaderManager::get();

        const auto & macros = getShaderMacros(_stage, key.m_flags);

        auto * shader = sm->compile(_api, m_file, entryPoint, _stage, macros);

        m_variants[key] = shader;

        return shader;
    }

    //--------------------------------------------------------------------------------------
    void HLSLDesc::declFlag(uint _index, ShaderStageFlags _stages, const core::string & _define)
    {
        VG_ASSERT(_index < countof(m_flagDescs));
        auto & desc = m_flagDescs[_index];
        VG_ASSERT(!desc.isInitialized(), "Index %u for shader \"%s\" is already used", _index, m_file.c_str());
        desc = ShaderFlagDesc(_stages, _define);
    }

    //--------------------------------------------------------------------------------------
    void HLSLDesc::declFlags(core::uint _startIndex, ShaderStageFlags _stages, const core::vector<core::string> & _defines)
    {
        VG_ASSERT(_defines.size() > 0);
        const uint bits = (uint)log2(_defines.size()) + 1;

        VG_ASSERT((_startIndex + bits - 1) < countof(m_flagDescs));

        for (uint i = _startIndex; i < _startIndex + bits; ++i)
            VG_ASSERT(!m_flagDescs[i].isInitialized(), "Bit %u for shader \"%s\" is already used", i, m_file.c_str());

        auto & desc = m_flagDescs[_startIndex];
        desc = ShaderFlagDesc(_stages, _defines);
    }

    //--------------------------------------------------------------------------------------
    // Keep only the flags that are relevant for the shader stage
    //--------------------------------------------------------------------------------------
    HLSLDesc::VariantKey HLSLDesc::computeVariantKey(ShaderStage _stage, ShaderKey::EntryPoint _index, ShaderKey::Flags _flags) const
    {
        ShaderKey::Flags flags = 0x0;
        for (uint i = 0; i < countof(m_flagDescs); ++i)
        {
            const auto & desc = m_flagDescs[i];
            if (desc.isInitialized())
            {
                const uint mask = desc.getBitMask();

                if (asBool(desc.m_stages & ShaderStageFlags(1 << (uint)_stage)))
                    flags |= _flags & (mask<<i);
            }
        }

        return VariantKey(_stage, _index, flags);
    }

    //--------------------------------------------------------------------------------------
    // Build macros from shader key flags
    //--------------------------------------------------------------------------------------
    core::vector<core::pair<core::string, core::uint>> HLSLDesc::getShaderMacros(ShaderStage _stage, ShaderKey::Flags _flags) const
    {
        core::vector<core::pair<core::string, core::uint>> macros;

        for (uint i = 0; i < countof(m_flagDescs); ++i)
        {
            const auto & desc = m_flagDescs[i];
            if (desc.isInitialized())
            {
                const uint mask = desc.getBitMask();
                const uint value = (_flags >> i) & mask;
                VG_ASSERT(value < desc.m_defines.size());
                if (!desc.m_defines[value].empty())
                {
                    VG_ASSERT(asBool(desc.m_stages & ShaderStageFlags(1 << (uint)_stage)), "#define \"%s\" shall not be used at %s stage", desc.m_defines[value].c_str(), asString(_stage).c_str());
                    macros.push_back({ desc.m_defines[value], 1 });
                }
            }
        }
        
        return macros;
    }

    //--------------------------------------------------------------------------------------
    HLSLDesc::Technique & HLSLDesc::declTechnique(const core::string & _name)
    {
        m_techniques.push_back({});
        Technique & technique = m_techniques.back();
        technique.name = _name;
        return technique;
    }

    //--------------------------------------------------------------------------------------
    HLSLDesc::Technique & HLSLDesc::declTechnique(const core::string & _name, ShaderKey::VS _vs, ShaderKey::PS _ps)
    {
        auto & technique = declTechnique(_name);
        {
            technique.vs = _vs;
            technique.ps = _ps;
        }
        return technique;
    }

    //--------------------------------------------------------------------------------------
    const core::vector<HLSLDesc::Technique> & HLSLDesc::getTechniques() const
    {
        return m_techniques;
    }

    //--------------------------------------------------------------------------------------
    void HLSLDesc::reset()
    {
        for (auto & pair : m_variants)
            VG_SAFE_RELEASE(pair.second);
    }

    //--------------------------------------------------------------------------------------
    bool HLSLDesc::isValidFlagValue(core::uint _index, core::uint _value) const
    {
        const uint bits = _value ? (uint)log2(_value) + 1 : 1;
        if (m_flagDescs[_index].m_bits < bits)
            return false;

        return true;
    }
}