#pragma once

// Should match declaration in "samplers.hlsli"
vg_enum_class_ns(vg::gfx, Sampler, core::u8,
    NearestClamp = 0,
    NearestRepeat,
    LinearClamp,
    LinearRepeat,
    AnisotropicClamp,
    AnisotropicRepeat,
    ShadowCmp
);
