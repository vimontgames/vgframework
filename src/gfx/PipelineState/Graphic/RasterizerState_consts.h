#pragma once

namespace vg::gfx
{
    enum class FillMode : core::u8
    {
        Solid = 0,
        Wireframe
    };

    enum class CullMode : core::u8
    {
        None = 0,
        Back,
        Front
    };

    enum class Orientation : core::u8
    {
        ClockWise = 0,
        CounterClockWise
    };

    enum class DepthClip : core::u8
    {
        Enable = 1,
        Disable
    };

    enum class DepthBias : core::u8
    {
        None,
        Small,
        Medium,
        Big
    };
}