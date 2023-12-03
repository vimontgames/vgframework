#pragma once

#include "core/Component/Component.h"

namespace vg::engine
{
    class IRigidBodyComponent : public core::Component
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IRigidBodyComponent, core::Component);
    };
}