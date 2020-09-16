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

#include VG_GFXAPI_HEADER(ShaderCompiler)

namespace vg::graphics::driver
{
    class ShaderCompiler : public VG_GFXAPI::ShaderCompiler
    {
        using super = VG_GFXAPI::ShaderCompiler;

    public:
        ShaderCompiler();
        ~ShaderCompiler();

        Shader * compile(const core::string & _file, const core::string & _entryPoint, ShaderStage _stage, core::vector<core::pair<core::string, core::uint>> & _macros);

    private:

    };
}

#endif // VG_ENABLE_SHADER_COMPILATION