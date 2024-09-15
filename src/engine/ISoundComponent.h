#pragma once

#include "core/Component/Component.h"

namespace vg::engine
{
    class ISoundComponent : public core::Component
    {
    public:
        VG_CLASS_DECL_ABSTRACT(ISoundComponent, core::Component);
    };
}