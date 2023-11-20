#pragma once

#include "core/Model/Model.h"

namespace vg::renderer
{
    class IAnimation : public core::Model
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(IAnimation, core::Model)
    };
}