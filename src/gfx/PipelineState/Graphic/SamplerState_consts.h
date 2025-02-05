#pragma once

namespace vg::gfx
{
    // Should match declaration in "samplers.hlsli"
    vg_enum_class(vg::gfx, Sampler, core::u8,
        NearestClamp = 0,
        NearestRepeat,
        LinearClamp,
        LinearRepeat,
        AnisotropicClamp,
        AnisotropicRepeat,
        ShadowCmp
    );
}