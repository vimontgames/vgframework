#pragma once

#include "ShaderKey.h"
#include "gfx/IShaderManager.h"


namespace vg::gfx
{
    class Shader;
    class ShaderCompiler;
    class HLSLDesc;

    class ShaderManager : public IShaderManager, public core::Singleton<ShaderManager>
    {
    public:
        const char * getClassName() const final { return "ShaderManager"; }

        ShaderManager(const core::string & _shaderRootPath);
        ~ShaderManager();

        core::uint GetShaderCompiledCount() const final override;
        core::uint GetShaderWarningCount() const final override;
        core::uint GetShaderErrorCount() const final override;

        void update();
        void applyUpdate();

        void registerHLSL(const HLSLDesc & _hlslDesc);
        bool initShaderKey(ShaderKey & _key, const core::string & _file, const core::string & _technique);

        HLSLDesc * getHLSLDescriptor(ShaderKey::File _file);

        Shader * compile(API _api, const core::string & _file, const core::string & _entryPoint, ShaderStage _stage, const core::vector<core::pair<core::string, core::uint>> & _macros);

        const core::string &               getShaderRootPath() const { return m_shaderRootPath; }
        const core::vector<core::string> & getShaderRootFolders() const { return m_shaderRootFolders; }

    private:
        bool                        m_updateNeeded = false;
        core::string                m_shaderRootPath;
        core::vector<core::string>  m_shaderRootFolders;
        ShaderCompiler *            m_shaderCompiler = nullptr;
        core::vector<HLSLDesc>      m_shaderFileDescriptors;
        core::uint                  m_compiledCount = 0;
        core::uint                  m_warningCount = 0;
        core::uint                  m_errorCount = 0;
    };
}