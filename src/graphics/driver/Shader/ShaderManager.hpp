#include "ShaderManager.h"

namespace vg::graphics::driver
{
    //--------------------------------------------------------------------------------------
    ShaderManager::ShaderManager(const core::string & _shaderRootPath) :
        m_shaderRootPath(_shaderRootPath)
    {
        // Path must end with '\' or '/'
        if (m_shaderRootPath.back() != '/' && m_shaderRootPath.back() != '\\')
            m_shaderRootPath += "/";
    
        m_shaderCompiler = new ShaderCompiler();
    }

    //--------------------------------------------------------------------------------------
    ShaderManager::~ShaderManager()
    {
        VG_SAFE_DELETE(m_shaderCompiler);
    }

    //--------------------------------------------------------------------------------------
    void ShaderManager::registerHLSL(const HLSLDesc & _hlslDesc)
    {
        m_shaderFileDescriptors.push_back(_hlslDesc);
    }

    //--------------------------------------------------------------------------------------
    bool ShaderManager::initShaderKey(ShaderKey & _key, const core::string & _file, const core::string & _technique)
    {
        for (uint i = 0; i < m_shaderFileDescriptors.size(); ++i)
        {
            const HLSLDesc & desc = m_shaderFileDescriptors[i];
        
            if (!_file.compare(desc.getFile()))  
            {
                _key.file = i;

                const auto & techniques = desc.getTechniques();
                for (uint j = 0; j < techniques.size(); ++j)
                {
                    const auto & technique = techniques[j];

                    if (!_technique.compare(technique.name))
                    {
                        _key.vs = technique.vs;
                        _key.hs = technique.hs;
                        _key.ds = technique.ds;
                        _key.gs = technique.gs;
                        _key.ps = technique.ps;
                        _key.flags = technique.flags;

                        return true;
                    }

                    VG_ASSERT("Technique \"%s\" not found in file \"%s\"", _technique.c_str(), _file.c_str());
                    return false;
                }
            }
        }

        VG_ASSERT("File \"%s\" not found", _file.c_str());
        return false;
    }

    //--------------------------------------------------------------------------------------
    HLSLDesc * ShaderManager::getHLSLDescriptor(ShaderKey::File _file)
    {
        return &m_shaderFileDescriptors[_file];
    }

    //--------------------------------------------------------------------------------------
    Shader * ShaderManager::compile(const core::string & _file, const core::string & _entryPoint, ShaderStage _stage, vector<pair<string, uint>> & _macros)
    {
        return m_shaderCompiler->compile(m_shaderRootPath + _file, _entryPoint, _stage, _macros);
    }
}