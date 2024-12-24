#pragma once

#include "gfx/Device/Device_consts.h"
#include "gfx/Resource/Texture_consts.h"

namespace vg::renderer
{
    struct RendererCreationParams
    {
        gfx::DeviceParams device;
        bool profileStart = false;
        bool editor = false;
    };
}