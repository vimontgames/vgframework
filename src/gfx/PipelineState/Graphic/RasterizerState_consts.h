#pragma once

namespace vg::gfx
{
    vg_enum_class(vg::gfx, FillMode, core::u8,
        Solid = 0,
        Wireframe
    );

    vg_enum_class(vg::gfx, CullMode, core::u8,
        None = 0,
        Back,
        Front
    );

    vg_enum_class(vg::gfx, Orientation, core::u8,
        ClockWise = 0,
        CounterClockWise
    );

    vg_enum_class(vg::gfx, DepthClip, core::u8,
        Enable = 1,
        Disable
    );

    vg_enum_class(vg::gfx, DepthBias, core::u8,
        None,
        Small,
        Medium,
        Big
    );
}