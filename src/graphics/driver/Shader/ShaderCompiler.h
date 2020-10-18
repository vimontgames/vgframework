#pragma once

#ifdef VG_ENABLE_SHADER_COMPILATION

#include "core/Object/Object.h"
#include "core/Singleton/Singleton.h"
#include "graphics/driver/Shader/Shader_consts.h"

namespace vg::graphics::driver
{
    class Shader;

    namespace base
    {
        class ShaderCompiler : public core::Object
        {
        public:
            ShaderCompiler();
            ~ShaderCompiler();
        };
    }
}

#include "dxc/ShaderCompiler_dxc.h"

namespace vg::graphics::driver
{
    class ShaderCompiler : public dxc::ShaderCompiler
    {
        using super = dxc::ShaderCompiler;

    public:
        ShaderCompiler();
        ~ShaderCompiler();

        Shader * compile(API _api, const core::string & _file, const core::string & _entryPoint, ShaderStage _stage, const core::vector<core::pair<core::string, core::uint>> & _macros, core::string & _warningAndErrors);

    private:

    };
}

#endif // VG_ENABLE_SHADER_COMPILATION