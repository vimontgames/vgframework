#pragma once

#include "core/Component/Component.h"

namespace vg::engine
{
    class IPhysicsShapeComponent : public core::Component
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IPhysicsShapeComponent, core::Component);
    };
}