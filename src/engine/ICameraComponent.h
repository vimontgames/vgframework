#pragma once

#include "core/Component/Component.h"

namespace vg::engine
{
    class ICameraComponent : public core::Component
    {
    public:
        VG_CLASS_DECL_ABSTRACT(ICameraComponent, core::Component);

        virtual void SetViewportOffsetAndScale(core::float2 _offset, const core::float2 & _scale) = 0;
    };
}