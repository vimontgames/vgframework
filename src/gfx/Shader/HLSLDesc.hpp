#include "HLSLDesc.h"

namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    void GraphicsPrograms::release()
    {
        for (Shader *& shader : m_shaders)
            VG_SAFE_RELEASE(shader);
    }

    //--------------------------------------------------------------------------------------
    void ComputePrograms::release()
    {
        for (Shader *& shader : m_shaders)
            VG_SAFE_RELEASE(shader);
    }

    //--------------------------------------------------------------------------------------
    void RayTracingPrograms::release()
    {
        for (Shader *& shader : m_shaders)
            VG_SAFE_RELEASE(shader);
    }

    //--------------------------------------------------------------------------------------
    HLSLDesc::~HLSLDesc()
    {
        for (uint i = 0; i < countof(m_flagDescs); ++i)
            m_flagDescs[i].clear();

        // Graphics
        for (uint i = 0; i < countof(m_graphicsEntryPoint); ++i)
            m_graphicsEntryPoint[i].clear();

        m_graphicsTechniques.clear();

        for (auto & pair : m_graphicsVariants)
            pair.second.release();

        // Compute
        for (uint i = 0; i < countof(m_computeEntryPoint); ++i)
            m_computeEntryPoint[i].clear();

        m_computeTechniques.clear();

        for (auto & pair : m_computeVariants)
            pair.second.release();

        // RayTracing
        for (uint i = 0; i < countof(m_rayTracingEntryPoint); ++i)
            m_rayTracingEntryPoint[i].clear();

        m_rayTracingTechniques.clear();

        for (auto & pair : m_rayTracingVariants)
            pair.second.release();
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
    // Graphics
    //--------------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------------
    ShaderKey::EntryPoint HLSLDesc::addGraphicsEntryPoint(GraphicsStage _stage, const core::string & _entryPoint)
    {
        auto & stageEntryPoints = m_graphicsEntryPoint[asInteger(_stage)];
        
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
    VG_INLINE ShaderProgramType getShaderProgramType(GraphicsStage _stage)
    {
        VG_STATIC_ASSERT(asInteger(ShaderProgramType::Vertex  ) == asInteger(GraphicsStage::Vertex  ), "ShaderProgramType enum does not match GraphicsStage");
        VG_STATIC_ASSERT(asInteger(ShaderProgramType::Hull    ) == asInteger(GraphicsStage::Hull    ), "ShaderProgramType enum does not match GraphicsStage");
        VG_STATIC_ASSERT(asInteger(ShaderProgramType::Domain  ) == asInteger(GraphicsStage::Domain  ), "ShaderProgramType enum does not match GraphicsStage");
        VG_STATIC_ASSERT(asInteger(ShaderProgramType::Geometry) == asInteger(GraphicsStage::Geometry), "ShaderProgramType enum does not match GraphicsStage");
        VG_STATIC_ASSERT(asInteger(ShaderProgramType::Pixel   ) == asInteger(GraphicsStage::Pixel   ), "ShaderProgramType enum does not match GraphicsStage");

        return (ShaderProgramType)_stage;
    }

    //--------------------------------------------------------------------------------------
    bool HLSLDesc::getGraphicsPrograms(API _api, const ShaderKey & _key, GraphicsPrograms & _programs)
    {
        auto it = m_graphicsVariants.find(_key);
        if (m_graphicsVariants.end() != it)
        {
            _programs = it->second;
            bool valid = true;

            for (uint i = 0; i < core::enumCount<GraphicsStage>(); ++i)
            {
                const auto stage = (GraphicsStage)i;
                ShaderKey::EntryPoint index = _key.getEntryPointIndex(stage);
                if (ShaderKey::EntryPoint(-1) != index)
                {
                    if (!_programs.m_shaders[i])
                    {
                        valid = false;
                        break;
                    }
                }
            }

            if (valid)
                return true;
            else
                _programs.release();
        }

        auto * sm = ShaderManager::get();
        const auto & macros = getShaderMacros(_key.flags);

        for (uint i = 0; i < core::enumCount<GraphicsStage>(); ++i)
        {
            const auto stage = (GraphicsStage)i;
            ShaderKey::EntryPoint index = _key.getEntryPointIndex(stage);
            if (ShaderKey::EntryPoint(-1) != index)
            {
                _programs.m_shaders[i] = sm->compile(_api, m_file, getGraphicsEntryPoint(stage, index), getShaderProgramType(stage), macros);

                if (!_programs.m_shaders[i])
                {
                    _programs.release();
                    return false;
                }
            }
        }     

        m_graphicsVariants[_key] = _programs;

        return true;
    }

    //--------------------------------------------------------------------------------------
    const string & HLSLDesc::getGraphicsEntryPoint(GraphicsStage _stage, ShaderKey::EntryPoint _index) const
    {
        return m_graphicsEntryPoint[asInteger(_stage)][_index];
    }

    //--------------------------------------------------------------------------------------
    // Compute
    //--------------------------------------------------------------------------------------
    
    //--------------------------------------------------------------------------------------
    ComputeShaderKey::EntryPoint HLSLDesc::addComputeEntryPoint(ComputeStage _stage, const core::string & _entryPoint)
    {
        auto & stageEntryPoints = m_computeEntryPoint[asInteger(_stage)];

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
    VG_INLINE ShaderProgramType getShaderProgramType(ComputeStage _stage)
    {
        VG_STATIC_ASSERT(asInteger(ShaderProgramType::Compute) == asInteger(ComputeStage::Compute) + 5, "ShaderProgramType enum does not match ComputeStage + 5");
        return (ShaderProgramType)((core::uint)_stage + 5);
    }

    //--------------------------------------------------------------------------------------
    bool HLSLDesc::getComputePrograms(API _api, const ComputeShaderKey & _key, ComputePrograms & _programs)
    {
        auto it = m_computeVariants.find(_key);
        if (m_computeVariants.end() != it)
        {
            _programs = it->second;
            bool valid = true;

            for (uint i = 0; i < core::enumCount<ComputeStage>(); ++i)
            {
                const auto stage = (ComputeStage)i;
                ComputeShaderKey::EntryPoint index = _key.getEntryPointIndex(stage);
                if (ComputeShaderKey::EntryPoint(-1) != index)
                {
                    if (!_programs.m_shaders[i])
                    {
                        valid = false;
                        break;
                    }
                }
            }

            if (valid)
                return true;
            else
                _programs.release();
        }

        auto * sm = ShaderManager::get();
        const auto & macros = getShaderMacros(_key.flags);

        for (uint i = 0; i < core::enumCount<ComputeStage>(); ++i)
        {
            const auto stage = (ComputeStage)i;
            ComputeShaderKey::EntryPoint index = _key.getEntryPointIndex(stage);
            if (ComputeShaderKey::EntryPoint(-1) != index)
            {
                _programs.m_shaders[i] = sm->compile(_api, m_file, getComputeEntryPoint(stage, index), getShaderProgramType(stage), macros);

                if (!_programs.m_shaders[i])
                {
                    _programs.release();
                    return false;
                }
            }
        }

        m_computeVariants[_key] = _programs;

        return true;
    }

    //--------------------------------------------------------------------------------------
    const string & HLSLDesc::getComputeEntryPoint(ComputeStage _stage, ComputeShaderKey::EntryPoint _index) const
    {
        return m_computeEntryPoint[asInteger(_stage)][_index];
    }

    //--------------------------------------------------------------------------------------
    // RayTracing
    //--------------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------------
    RayTracingShaderKey::EntryPoint HLSLDesc::addRayTracingEntryPoint(RayTracingStage _stage, const core::string & _entryPoint)
    {
        auto & stageEntryPoints = m_rayTracingEntryPoint[asInteger(_stage)];

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
    ShaderKey::Flags HLSLDesc::declareFlag(uint _index, const core::string & _define)
    {
        VG_ASSERT(_index < countof(m_flagDescs));
        auto & desc = m_flagDescs[_index];
        if (!desc.m_define.empty())
            VG_ASSERT(desc.m_define == _define, "[Shader] Same Flag %u cannot be declared using different names");

        desc = ShaderFlagDesc(_define);
        return _index << 16;
    }

    //--------------------------------------------------------------------------------------
    // Build macros from shader key flags
    //--------------------------------------------------------------------------------------
    core::vector<core::pair<core::string, core::uint>> HLSLDesc::getShaderMacros(ShaderKey::Flags _flags) const
    {
        core::vector<core::pair<core::string, core::uint>> macros;

        u32 value = (u32)_flags;
        u32 ctlz = core::countl_zero(value);
        u32 index = 0;
        while (ctlz < 32)
        {
            index = 32-ctlz-1;
            const auto & desc = m_flagDescs[index];
            macros.push_back({ desc.m_define,1 });
            value &= ~(1<<index);
            ctlz = core::countl_zero(value);
        }
        
        return macros;
    }

    //--------------------------------------------------------------------------------------
    HLSLDesc::GraphicsTechnique & HLSLDesc::addGraphicsTechnique(const core::string & _name)
    {
        auto & technique = m_graphicsTechniques.push_empty();
        technique.desc = this;
        technique.name = _name;
        return technique;
    }

    //--------------------------------------------------------------------------------------
    const core::vector<HLSLDesc::GraphicsTechnique> & HLSLDesc::getGraphicsTechniques() const
    {
        return m_graphicsTechniques;
    }

    //--------------------------------------------------------------------------------------
    HLSLDesc::ComputeTechnique & HLSLDesc::addComputeTechnique(const core::string & _name)
    {
        auto & technique = m_computeTechniques.push_empty();
        technique.desc = this;
        technique.name = _name;
        return technique;
    }

    //--------------------------------------------------------------------------------------
    const core::vector<HLSLDesc::ComputeTechnique> & HLSLDesc::getComputeTechniques() const
    {
        return m_computeTechniques;
    }

    //--------------------------------------------------------------------------------------
    HLSLDesc::RayTracingTechnique & HLSLDesc::addRayTracingTechnique(const core::string & _name)
    {
        auto & technique = m_rayTracingTechniques.push_empty();
        technique.desc = this;
        technique.name = _name;
        return technique;
    }

    //--------------------------------------------------------------------------------------
    const core::vector<HLSLDesc::RayTracingTechnique> & HLSLDesc::getRayTracingTechniques() const
    {
        return m_rayTracingTechniques;
    }

    //--------------------------------------------------------------------------------------
    void HLSLDesc::release()
    {
        for (auto & pair : m_graphicsVariants)
            pair.second.release();

        for (auto & pair : m_computeVariants)
            pair.second.release();

        for (auto & pair : m_rayTracingVariants)
            pair.second.release();
    }
}