#pragma once

#include "core/Object/Object.h"

namespace vg::core
{
    class Blob;
}

namespace vg::gfx
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

namespace vg::gfx
{
    class Shader : public VG_GFXAPI::Shader
    {
        using super = VG_GFXAPI::Shader;

    public:
        const char * GetClassName() const final { return "Shader"; }

        Shader(const core::Blob & _bytecode);
        ~Shader();
    };
}