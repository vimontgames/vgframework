#pragma once

namespace vg::renderer
{
    enum MaterialFlags : core::u32
    {
        AlphaTest   = 0x00000001,
        AlphaBlend  = 0x00000002
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