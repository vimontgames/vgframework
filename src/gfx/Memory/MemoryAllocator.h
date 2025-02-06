#pragma once

#include "core/Object/Object.h"

namespace vg::gfx
{
    namespace base
    {
        class MemoryAllocator : public core::Object
        {
        public:
            MemoryAllocator();
            ~MemoryAllocator();
        };
    }
}

#ifdef VG_DX12
#include "dx12/MemoryAllocator_dx12.h"
#elif defined(VG_VULKAN)
#include "vulkan/MemoryAllocator_vulkan.h"
#else
#error Undefined GFXAPI
#endif

namespace vg::gfx
{
    class MemoryAllocator : public VG_GFXAPI::MemoryAllocator
    {
        using super = VG_GFXAPI::MemoryAllocator;

    public:
        const char * GetClassName() const final { return "MemoryAllocator"; }

        MemoryAllocator();
        ~MemoryAllocator();
    };
}