#pragma once

#include "core/Object/Object.h"

namespace vg::graphics::driver
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

#include VG_GFXAPI_HEADER(MemoryAllocator)

namespace vg::graphics::driver
{
    class MemoryAllocator : public VG_GFXAPI::MemoryAllocator
    {
        using super = VG_GFXAPI::MemoryAllocator;

    public:
        const char * getClassName() const final { return "MemoryAllocator"; }

        MemoryAllocator();
        ~MemoryAllocator();
    };
}