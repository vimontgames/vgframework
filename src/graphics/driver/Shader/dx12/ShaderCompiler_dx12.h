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
        driver::Shader * compile(const core::string & _file, const core::string & _entryPoint, ShaderStage _stage);
        
    private:
        IDxcLibrary *           m_d3d12dxcLibrary = nullptr;
        IDxcCompiler *          m_d3d12dxcCompiler = nullptr;
        IDxcIncludeHandler *    m_d3d12dxcIncludeHandler = nullptr;
    };
}