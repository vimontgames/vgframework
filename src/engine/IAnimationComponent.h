#pragma once

#include "core/Component/Component.h"

namespace vg::engine
{
    class IAnimationResource;

    class IAnimationComponent : public core::Component
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IAnimationComponent, core::Component);

        virtual IAnimationResource * GetAnimation(const core::string & _name) = 0;
    };
}