#pragma once

vg_enum_class_ns(vg::gfx, TextureImporterType, core::u8,
    Auto = 0,

    Texture1D = 1,
    Texture2D = 2,
    Texture3D = 3,
    Cubemap = 4
);

vg_enum_class_ns(vg::gfx, TextureImporterFormat, core::u8,
    Auto = 0,

    RGBA8,
    RGBA16,
    RGBA32f
);

vg_enum_class_ns(vg::gfx, TextureImporterMip, core::u8,
    TextureImporterMip_Auto = 0,

    TextureImporterMip_1 = 1,
    TextureImporterMip_2 = 2,
    TextureImporterMip_3 = 3,
    TextureImporterMip_4 = 4,
    TextureImporterMip_5 = 5,
    TextureImporterMip_6 = 6,
    TextureImporterMip_7 = 7,
    TextureImporterMip_8 = 8,
    TextureImporterMip_9 = 9,
    TextureImporterMip_10 = 10,
    TextureImporterMip_11 = 11,
    TextureImporterMip_12 = 12,
    TextureImporterMip_13 = 13,
    TextureImporterMip_14 = 14,
    TextureImporterMip_15 = 15
);

vg_enum_class_ns(vg::gfx, Downscale, core::u8,
    None = 0,

    Downscale_2,
    Downscale_4,
    Downscale_8,
    Downscale_16
);

namespace vg::gfx
{
    struct TextureImporterSettings
    {
        TextureImporterType             m_importerType = TextureImporterType::Auto;
        TextureImporterFormat           m_importerFormat = TextureImporterFormat::Auto;
        bool                            m_sRGB = true;
        TextureImporterMip              m_mipLevelCount = TextureImporterMip::TextureImporterMip_Auto;
        Downscale                       m_downscale = Downscale::None;
    };
}