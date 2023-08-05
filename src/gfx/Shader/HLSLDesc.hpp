#include "HLSLDesc.h"

namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    HLSLDesc::~HLSLDesc()
    {
        for (uint i = 0; i < core::enumCount<ShaderStage>(); ++i)
        {
            for (auto & pair : m_shader[i])
                VG_SAFE_RELEASE(pair.second);
            m_shader[i].clear();
        }

        m_techniques.clear();
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
    Shader * HLSLDesc::getVS(API _api, ShaderKey::VS _vs)
    {
        return getShader(_api, ShaderStage::Vertex, _vs);
    }    

    //--------------------------------------------------------------------------------------
    Shader * HLSLDesc::getHS(API _api, ShaderKey::HS _hs)
    {
        return getShader(_api, ShaderStage::Hull, _hs);
    }

    //--------------------------------------------------------------------------------------
    Shader * HLSLDesc::getDS(API _api, ShaderKey::DS _ds)
    {
        return getShader(_api, ShaderStage::Domain, _ds);
    }

    //--------------------------------------------------------------------------------------
    Shader * HLSLDesc::getGS(API _api, ShaderKey::GS _gs)
    {
        return getShader(_api, ShaderStage::Geometry, _gs);
    }

    //--------------------------------------------------------------------------------------
    Shader * HLSLDesc::getPS(API _api, ShaderKey::PS _ps)
    {
        return getShader(_api, ShaderStage::Pixel, _ps);
    }

    //--------------------------------------------------------------------------------------
    core::uint HLSLDesc::addShader(ShaderStage _stage, const core::string & _entryPoint)
    {
        auto & shaders = m_shader[asInteger(_stage)];
        shaders.push_back(core::pair<core::string, Shader*>(_entryPoint, nullptr));
        return ShaderKey::VS(shaders.size() - 1);
    }

    //--------------------------------------------------------------------------------------
    Shader * HLSLDesc::getShader(API _api, ShaderStage _stage, core::uint _index)
    {
        auto & pair = m_shader[asInteger(_stage)][_index];
        if (nullptr == pair.second)
        {
            auto * sm = ShaderManager::get();
            core::vector<core::pair<core::string, core::uint>> macros; // TODO: build macros from shader key flags
            pair.second = sm->compile(_api, m_file, pair.first, _stage, macros);
        }
        return pair.second;
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
    const core::vector<HLSLDesc::Technique> & HLSLDesc::getTechniques() const
    {
        return m_techniques;
    }

    //--------------------------------------------------------------------------------------
    void HLSLDesc::resetShaders()
    {
        for (uint j = 0; j < core::enumCount<ShaderStage>(); ++j)
        {
            for (auto & pair : m_shader[j])
                VG_SAFE_RELEASE(pair.second);
        }
    }
}