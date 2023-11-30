#pragma once
#include "core/Component/Component.h"

namespace vg::core
{
    class IBehaviour : public Component
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IBehaviour, Component);
    };
}