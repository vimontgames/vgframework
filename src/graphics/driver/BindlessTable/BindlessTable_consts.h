#pragma once

namespace vg::graphics::driver
{
    using BindlessHandle = core::u16;

    using BindlessTextureHandle = BindlessHandle;
    using BindlessBufferHandle = BindlessHandle;
    using BindlessUAVHandle = BindlessHandle;

    constexpr core::uint max_bindless_elements = 65536;

    constexpr BindlessHandle invalidBindlessTextureHandle = (BindlessHandle)max_bindless_elements-1;
    constexpr BindlessHandle invalidBindlessBufferHandle = (BindlessHandle)max_bindless_elements-2;
    constexpr BindlessHandle invalidBindlessUAVHandle = (BindlessHandle)max_bindless_elements-3;
}