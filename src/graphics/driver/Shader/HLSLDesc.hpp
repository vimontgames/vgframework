#include "HLSLDesc.h"

namespace vg::graphics::driver
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
    Shader * HLSLDesc::getVS(ShaderKey::VS _vs)
    {
        return getShader(ShaderStage::Vertex, _vs);
    }    

    //--------------------------------------------------------------------------------------
    Shader * HLSLDesc::getPS(ShaderKey::PS _ps)
    {
        return getShader(ShaderStage::Pixel, _ps);
    }

    //--------------------------------------------------------------------------------------
    core::uint HLSLDesc::addShader(ShaderStage _stage, const core::string & _entryPoint)
    {
        auto & shaders = m_shader[asInteger(_stage)];
        //VG_ASSERT(shaders.end() == std::find(shaders.begin(), shaders.end(), _entryPoint));
        shaders.push_back(core::pair<core::string, Shader*>(_entryPoint, nullptr));
        return ShaderKey::VS(shaders.size() - 1);
    }

    //--------------------------------------------------------------------------------------
    Shader * HLSLDesc::getShader(ShaderStage _stage, core::uint _index)
    {
        auto & pair = m_shader[asInteger(_stage)][_index];
        if (nullptr == pair.second)
        {
            auto * sm = ShaderManager::get();
            pair.second = sm->compile(m_file, pair.first, _stage);
        }
        return pair.second;
    }

    //--------------------------------------------------------------------------------------
    HLSLDesc::Technique & HLSLDesc::addTechnique(const core::string & _name)
    {
        //VG_ASSERT(m_techniques.end() == std::find(m_techniques.begin(), m_techniques.end(), _name));
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
}