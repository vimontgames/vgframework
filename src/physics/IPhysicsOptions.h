#pragma once
#include "core/Object/Object.h"

namespace vg::physics
{
    class IPhysicsOptions : public core::Object
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IPhysicsOptions, core::Object);
    };
}