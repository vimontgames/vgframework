#pragma once

namespace vg::gfx
{
    vg_enum_class(TextureImporterFormat, core::u8,
        Automatic = 0,

        R8G8B8A8            = (core::u8)gfx::PixelFormat::R8G8B8A8_unorm_sRGB,
        R16G16B16A16_float  = (core::u8)gfx::PixelFormat::R16G16B16A16_float
    );

    vg_enum_class(MipLevelCount, core::u8,
        Automatic           = 0,
        MipLevelCount_1     = 1,
        MipLevelCount_2     = 2,
        MipLevelCount_3     = 3,
        MipLevelCount_4     = 4,
        MipLevelCount_5     = 5,
        MipLevelCount_6     = 6,
        MipLevelCount_7     = 7,
        MipLevelCount_8     = 8
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
        TextureImporterFormat   m_importerFormat = TextureImporterFormat::Automatic;
        bool                    m_sRGB = true;
        MipLevelCount           m_mipLevelCount = MipLevelCount::Automatic;
        Downscale               m_downscale = Downscale::None;
    };
}