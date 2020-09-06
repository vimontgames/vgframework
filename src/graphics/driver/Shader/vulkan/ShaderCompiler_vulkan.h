#pragma once

#include "graphics/driver/Shader/ShaderCompiler.h"

namespace vg::graphics::driver::vulkan
{
    class ShaderCompiler : public base::ShaderCompiler
    {
        using super = base::ShaderCompiler;

    public:
        ShaderCompiler();
        ~ShaderCompiler();

    protected:
        driver::Shader * compile(const core::string & _file, const core::string & _entryPoint, ShaderStage _stage);
    };
}