#pragma once

#include "core/Component/Component.h"

namespace vg::engine
{
    class IAnimationComponent : public core::Component
    {
    public:
        VG_CLASS_DECL_VIRTUAL(IAnimationComponent, core::Component)
    };
}