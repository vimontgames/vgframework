#include "HLSLDesc.h"

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
    ShaderKey::VS HLSLDesc::addVS(const core::string & _vsEntryPoint)
    {
        return (ShaderKey::VS)addShader(ShaderStage::Vertex, _vsEntryPoint);
    }

    //--------------------------------------------------------------------------------------
    ShaderKey::PS HLSLDesc::addPS(const core::string & _psEntryPoint)
    {
        return (ShaderKey::PS)addShader(ShaderStage::Pixel, _psEntryPoint);
    }

    //--------------------------------------------------------------------------------------
    ComputeShaderKey::CS HLSLDesc::addCS(const core::string & _csEntryPoint)
    {
        return (ComputeShaderKey::CS)addShader(ShaderStage::Compute, _csEntryPoint);
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
    ShaderKey::EntryPoint HLSLDesc::addShader(ShaderStage _stage, const core::string & _entryPoint)
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
        
        VariantKey key(_stage, _index, _flags);

        auto it = m_variants.find(key);

        if (m_variants.end() != it)
        {
            if (nullptr != it->second)
                return it->second;
        }

        auto * sm = ShaderManager::get();

        const auto & macros = getShaderMacros(_stage, _flags);

        auto * shader = sm->compile(_api, m_file, entryPoint, _stage, macros);

        m_variants[key] = shader;

        return shader;
    }

    //--------------------------------------------------------------------------------------
    void HLSLDesc::addFlag(uint _index, ShaderStageFlags _stages, const core::string & _define)
    {
        VG_ASSERT(_index < countof(m_flagDescs));
        auto & desc = m_flagDescs[_index];
        VG_ASSERT((ShaderStageFlags)0x0 == desc.m_stages);
        desc = ShaderFlagDesc(_stages, _define);
    }

    //--------------------------------------------------------------------------------------
    // Build macros from shader key flags
    //--------------------------------------------------------------------------------------
    core::vector<core::pair<core::string, core::uint>> HLSLDesc::getShaderMacros(ShaderStage _stage, ShaderKey::Flags _flags) const
    {
        core::vector<core::pair<core::string, core::uint>> macros;

        u32 value = (u32)_flags;
        u32 ctlz = core::clz(value);
        u32 index = 0;
        while (ctlz < 32)
        {
            index = 32-ctlz-1;
            const auto & desc = m_flagDescs[index];
            macros.push_back({ desc.m_define, 1 });
            value &= ~(1<<index);
            ctlz = core::clz(value);
        }
        
        return macros;
    }

    //--------------------------------------------------------------------------------------
    HLSLDesc::Technique & HLSLDesc::addTechnique(const core::string & _name)
    {
        m_techniques.push_back({});
        Technique & technique = m_techniques.back();
        technique.name = _name;
        return technique;
    }

    //--------------------------------------------------------------------------------------
    HLSLDesc::Technique & HLSLDesc::addTechnique(const core::string & _name, ShaderKey::VS _vs, ShaderKey::PS _ps, ShaderKey::Flags _flag)
    {
        auto & technique = addTechnique(_name);
        {
            technique.vs = _vs;
            technique.ps = _ps;
            technique.addFlag(_flag);
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
}