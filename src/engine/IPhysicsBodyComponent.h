#pragma once

#include "core/Component/Component.h"

namespace vg::engine
{
    class IPhysicsBodyComponent : public core::Component
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IPhysicsBodyComponent, core::Component);
    };
}