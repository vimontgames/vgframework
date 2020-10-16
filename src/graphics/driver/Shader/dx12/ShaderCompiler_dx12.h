#pragma once

#include "graphics/driver/Shader/ShaderCompiler.h"

namespace vg::graphics::driver::dx12
{
    class ShaderCompiler : public base::ShaderCompiler
    {
        using super = base::ShaderCompiler;

    public:
        ShaderCompiler();
        ~ShaderCompiler();

    protected:
        driver::Shader * compile(const core::string & _path, const core::string & _entryPoint, ShaderStage _stage, const core::vector<core::pair<core::string, core::uint>> & _macros, string & _warningAndErrors);
        
    private:
        IDxcLibrary *           m_d3d12dxcLibrary = nullptr;
        IDxcCompiler *          m_d3d12dxcCompiler = nullptr;
        IDxcIncludeHandler *    m_d3d12dxcIncludeHandler = nullptr;
    };
}