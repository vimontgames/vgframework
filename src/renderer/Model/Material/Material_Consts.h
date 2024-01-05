#pragma once

namespace vg::renderer
{
    enum SurfaceType : core::u8
    {
        Opaque      = 0,
        AlphaTest,
        AlphaBlend
    };

    enum class MaterialTextureType : core::u8
    {
        Albedo = 0,
        Normal,
        PBR
    };

    enum class MaterialColorType : core::u8
    {
        BaseColor = 0,
        EmissiveColor
    };

    enum class MaterialFloatType : core::u8
    {
        NormalStrength = 0,
        Occlusion,
        Roughness,
        Metalness
    };
}