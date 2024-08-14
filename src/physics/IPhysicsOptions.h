#pragma once
#include "core/Options/Options.h"
#include "physics/Physics_Consts.h"

namespace vg::physics
{
    class IPhysicsOptions : public core::Options
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IPhysicsOptions, core::Options);
    };
}