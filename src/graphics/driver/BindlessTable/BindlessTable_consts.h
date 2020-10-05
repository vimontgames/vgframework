#pragma once

namespace vg::graphics::driver
{
    using BindlessHandle = core::u16;

    using BindlessTextureSrvHandle = BindlessHandle;
    using BindlessTextureUAVHandle = BindlessHandle;
    using BindlessBufferSrvHandle  = BindlessHandle;
    using BindlessBufferUAVHandle  = BindlessHandle;

    enum ReservedSlot : core::u32
    {
        None = 0x80000000
    };
}