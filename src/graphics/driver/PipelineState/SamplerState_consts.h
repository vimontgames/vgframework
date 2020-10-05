#pragma once

namespace vg::graphics::driver
{
    // Should match declaration in "samplers.hlsli"
    enum class Sampler : core::u8
    {
        NearestClamp = 0,
        NearestRepeat,
        LinearClamp,
        LinearRepeat
    };
}