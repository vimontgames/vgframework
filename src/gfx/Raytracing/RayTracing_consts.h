#pragma once

namespace vg::gfx
{
    enum class SurfaceType : core::u8
    {
        Opaque = 0,
        AlphaTest,
        AlphaBlend
    };

    enum class BLASUpdateType
    {
        Static = 0, // Slower to build but faster to render
        Dynamic     // Faster to build but slower to render
    };
}