#pragma once

#include "gfx/Raytracing/RayTracing_consts.h"
#include "shaders/system/material_consts.hlsli"

namespace vg::renderer
{
    static const auto s_MaxMaterialCount = 65536;
    using GPUMaterialDataIndex = u16;

    vg_enum_class(vg::renderer, MaterialTextureType, core::u8,
        Albedo = 0,
        Normal,
        PBR,
        Emissive
    );

    vg_enum_class(vg::renderer, MaterialColorType, core::u8,
        BaseColor = 0,
        EmissiveColor
    );

    vg_enum_class(vg::renderer, MaterialFloatType, core::u8,
        NormalStrength = 0,
        Occlusion,
        Roughness,
        Metalness
    );
}