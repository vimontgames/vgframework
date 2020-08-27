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

#include VG_GRAPHICSAPI_HEADER(MemoryAllocator)

namespace vg::graphics::driver
{
    class MemoryAllocator : public VG_GRAPHICSAPI::MemoryAllocator
    {
        using super = VG_GRAPHICSAPI::MemoryAllocator;

    public:
        MemoryAllocator();
        ~MemoryAllocator();
    };
}