#pragma once

#include "graphics/driver/Resource/Resource_consts.h"

namespace vg::graphics::driver
{
    enum class BufferFlags : core::u16
    {
        None = 0x0000
    };

    class BufferDesc
    {
    public:
        BufferDesc(Usage _usage, BindFlags _bindFlags, CPUAccessFlags _cpuAccessFlags, BufferFlags _flags, core::u32 _elementSize, core::u32 _elementCount = 1) :
            resource(_usage, _bindFlags, _cpuAccessFlags),
            flags(_flags),
            elementSize(_elementSize),
            elementCount(_elementCount)
        {

        }

        core::u32 size() const { return elementSize * elementCount; }

        inline bool isShaderResource() const { return resource.isShaderResource(); }
        inline bool isDynamicResource() const { return resource.isDynamicResource(); }

        ResourceDesc    resource;
        BufferFlags     flags;
        core::u32		elementSize;
        core::u32		elementCount;
    };
}