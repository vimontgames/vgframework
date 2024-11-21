#pragma once

#include "core/Object/Object.h"

namespace vg::renderer
{
    class ICameraSettings : public core::Object
    {
    public:
        VG_CLASS_DECL_ABSTRACT(ICameraSettings, core::Object);
    };
}