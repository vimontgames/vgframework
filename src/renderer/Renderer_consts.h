#pragma once

#include "gfx/Device/Device_consts.h"
#include "gfx/Resource/Texture_consts.h"

namespace vg::renderer
{
    vg_enum_class(vg::renderer, OutlineCategory, core::u8,
        None = 0,
        SelectedObject,
        SelectedPrefab,
        SelectedPrefabObject,
        User_4,
        User_5,
        User_6,
        User_7,
        User_8,
        User_9,
        User_10,
        User_11,
        User_12,
        User_13,
        User_14,
        User_15
    );

    struct RendererCreationParams
    {
        gfx::DeviceParams device;
        bool profileStart = false;
        bool editor = false;
    };
}