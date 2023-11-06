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
        BufferDesc(Usage _usage = Usage::Default, BindFlags _bindFlags = BindFlags::ShaderResource, CPUAccessFlags _cpuAccessFlags = CPUAccessFlags::None, BufferFlags _flags = BufferFlags::None, core::u32 _elementSize = 4, core::u32 _elementCount = 1) :
            resource(_usage, _bindFlags, _cpuAccessFlags),
            flags(_flags),
            elementSize(_elementSize),
            elementCount(_elementCount)
        {

        }

        core::u32 size() const { return elementSize * elementCount; }

		inline bool testBindFlags(BindFlags _flags) const { return resource.testBindFlags(_flags); }

        ResourceDesc    resource;
        BufferFlags     flags;
        core::u32		elementSize;
        core::u32		elementCount;
    };
}