#pragma once

#include "core/Types/Types.h"
#include "gfx/Resource/Texture_consts.h"

namespace vg::renderer
{
    enum class Downscale : core::u8
    {
        None = 0,
        Half,
        Quarter
    };

    struct TextureImportSettings
    {
        bool                m_overridePixelFormat = false;
        gfx::PixelFormat    m_pixelFormat = gfx::PixelFormat::R8G8B8A8_unorm_sRGB;

        bool                m_overrideDownscale = false;
        Downscale           m_downscale = Downscale::None;
    };
}