#pragma once

namespace vg::graphics::driver
{
    using BindlessHandle = core::u16;

    using BindlessTextureHandle = BindlessHandle;
    using BindlessBufferHandle = BindlessHandle;
    using BindlessUAVHandle = BindlessHandle;

    constexpr core::uint max_bindless_elements = 65536;

    constexpr BindlessHandle invalidBindlessTextureSRVHandle     = 65535;
    constexpr BindlessHandle invalidBindlessTextureUAVHandle     = 65534;
    constexpr BindlessHandle invalidBindlessBufferSRVHandle      = 65533;
    constexpr BindlessHandle invalidBindlessBufferUAVHandle      = 65532;
    constexpr BindlessHandle invalidBindlessConstantBufferHandle = 65531;

    enum ReservedSlot : core::u32
    {
        None = 0x80000000
    };
}