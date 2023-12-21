#pragma once

#include "gfx/Resource/Resource_consts.h"

namespace vg::gfx
{
    enum class BufferFlags : core::u16
    {
        None = 0x0000
    };

    class BufferDesc
    {
    public:
        BufferDesc(Usage _usage = Usage::Default, BindFlags _bindFlags = BindFlags::ShaderResource, CPUAccessFlags _cpuAccessFlags = CPUAccessFlags::None, BufferFlags _flags = BufferFlags::None, core::u32 _elementSize = 4, core::u32 _elementCount = 1, core::u32 _alignment = 0) :
            resource(_usage, _bindFlags, _cpuAccessFlags),
            flags(_flags),
            elementSize(_elementSize),
            elementCount(_elementCount),
            alignment(_alignment)
        {

        }

        core::u32 getSize() const { return elementSize * elementCount; }
        core::u32 getElementCount() const { return elementCount; }
        core::u32 getElementSize() const { return elementSize; }
        core::u32 getAlignment() const { return alignment; }

		inline bool testBindFlags(BindFlags _flags) const { return resource.testBindFlags(_flags); }

        ResourceDesc    resource;
        BufferFlags     flags;
        core::u32		elementSize;
        core::u32		elementCount;
        core::u32       alignment;
    };
}