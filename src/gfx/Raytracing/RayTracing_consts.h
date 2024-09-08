#pragma once

namespace vg::gfx
{
    vg_enum_class(SurfaceType, core::u8,
        Opaque = 0,
        AlphaTest,
        AlphaBlend
    );

    vg_enum_class(BLASUpdateType, core::u8,
        Static = 0, // Slower to build but faster to render
        Dynamic     // Faster to build but slower to render
    );
}