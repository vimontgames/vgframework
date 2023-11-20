#pragma once

#include "IAnimation.h"

namespace vg::renderer
{
    class ISkeletalAnimation : IAnimation
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(ISkeletalAnimation, IAnimation)
    };
}