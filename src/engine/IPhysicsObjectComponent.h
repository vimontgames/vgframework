#pragma once

#include "core/Component/Component.h"

namespace vg::engine
{
    class IPhysicsObjectComponent : public core::Component
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IPhysicsObjectComponent, core::Component);
    };
}