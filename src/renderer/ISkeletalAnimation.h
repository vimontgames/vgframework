#pragma once

#include "IAnimation.h"

namespace vg::renderer
{
    class ISkeletalAnimation : public IAnimation
    {
    public:
        VG_CLASS_DECL_VIRTUAL(ISkeletalAnimation, IAnimation)
    };
}