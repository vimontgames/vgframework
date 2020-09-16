#include "ShaderKey.h"

#pragma once

namespace vg::graphics::driver
{
    class Shader;
    class ShaderCompiler;
    class HLSLDesc;

    class ShaderManager : public core::Singleton<ShaderManager>
    {
    public:
        ShaderManager(const core::string & _shaderRootPath);
        ~ShaderManager();

        void registerHLSL(const HLSLDesc & _hlslDesc);
        bool initShaderKey(ShaderKey & _key, const core::string & _file, const core::string & _technique);

        HLSLDesc * getHLSLDescriptor(ShaderKey::File _file);

        Shader * compile(const core::string & _file, const core::string & _entryPoint, ShaderStage _stage, core::vector<core::pair<core::string, core::uint>> & _macros);

    private:
        core::string            m_shaderRootPath;
        ShaderCompiler *        m_shaderCompiler = nullptr;
        core::vector<HLSLDesc>  m_shaderFileDescriptors;
    };
}