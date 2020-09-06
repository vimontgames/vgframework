#pragma once

#include "core/Object/Object.h"

namespace vg::core
{
    class Blob;
}

namespace vg::graphics::driver
{
    namespace base
    {
        class Shader : public core::Object
        {
        public:
            Shader(const core::Blob & _bytecode);
            ~Shader();
        };
    }
}

#include VG_GFXAPI_HEADER(Shader)

namespace vg::graphics::driver
{
    class Shader : public VG_GFXAPI::Shader
    {
        using super = VG_GFXAPI::Shader;

    public:
        Shader(const core::Blob & _bytecode);
        ~Shader();
    };
}