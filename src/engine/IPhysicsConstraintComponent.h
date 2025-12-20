#pragma once

#include "core/Component/Component.h"

namespace vg::engine
{
    class IPhysicsConstraintComponent : public core::Component
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IPhysicsConstraintComponent, core::Component);

    };
}