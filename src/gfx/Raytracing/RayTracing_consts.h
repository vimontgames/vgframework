#pragma once

namespace vg::gfx
{
    enum class BLASUpdateType
    {
        Static = 0, // Slower to build but faster to render
        Dynamic     // Faster to build but slower to render
    };
}