#pragma once

namespace vg::gfx
{
    vg_enum_class(TextureImporterType, core::u8,
        Automatic   = 0,

        Texture1D   = 1,
        Texture2D   = 2,
        Texture3D   = 3,
        Cubemap     = 4
    );

    vg_enum_class(TextureImporterFormat, core::u8,
        Automatic   = 0,

        RGBA8       = 1
    );

    vg_enum_class(TextureImporterMipLevelCount, core::u8,
        Automatic           = 0,

        MipLevelCount_1     = 1,
        MipLevelCount_2     = 2,
        MipLevelCount_3     = 3,
        MipLevelCount_4     = 4,
        MipLevelCount_5     = 5,
        MipLevelCount_6     = 6,
        MipLevelCount_7     = 7,
        MipLevelCount_8     = 8,
        MipLevelCount_9     = 9,
        MipLevelCount_10    = 10,
        MipLevelCount_11    = 11,
        MipLevelCount_12    = 12,
        MipLevelCount_13    = 13,
        MipLevelCount_14    = 14,
        MipLevelCount_15    = 15
    );

    vg_enum_class(Downscale , core::u8,
        None = 0,

        Downscale_2,
        Downscale_4,
        Downscale_8,
        Downscale_16
    );

    struct TextureImporterSettings
    {
        TextureImporterType     m_importerType = TextureImporterType::Automatic;
        TextureImporterFormat   m_importerFormat = TextureImporterFormat::Automatic;
        bool                    m_sRGB = true;
        TextureImporterMipLevelCount           m_mipLevelCount = TextureImporterMipLevelCount::Automatic;
        Downscale               m_downscale = Downscale::None;
    };
}