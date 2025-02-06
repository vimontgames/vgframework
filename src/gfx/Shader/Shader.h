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

#ifdef VG_DX12
#include "dx12/Shader_dx12.h"
#elif defined(VG_VULKAN)
#include "vulkan/Shader_vulkan.h"
#else
#error Undefined GFXAPI
#endif

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