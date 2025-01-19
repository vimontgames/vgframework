#pragma once

#include "core/Math/StorageVector.h"

namespace vg::renderer
{
    struct ColliderTriangle
    {
        core::StorageFloat3 v[3];
        core::u32 matID;
    };
}