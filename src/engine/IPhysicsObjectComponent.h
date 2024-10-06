#pragma once

#include "core/Component/Component.h"

namespace vg::physics
{
    enum class Category : core::u8;
    enum class CategoryFlag : core::u64;
}

namespace vg::engine
{
    class IPhysicsObjectComponent : public core::Component
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IPhysicsObjectComponent, core::Component);

        virtual void    SetCategory         (physics::Category _category) = 0;

        virtual void    EnableCollisionMask (bool _enable) = 0;
        virtual void    SetCollisionMask    (physics::CategoryFlag _mask) = 0;
    };
}