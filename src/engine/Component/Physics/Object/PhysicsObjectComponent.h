#pragma once

#include "engine/IPhysicsObjectComponent.h"
#include "core/Types/Types.h"

namespace vg::physics
{
    enum class Category : core::u8;
    enum class CategoryFlag : core::u64;
}

namespace vg::engine
{
    class PhysicsObjectComponent : public IPhysicsObjectComponent
    {
    public:
        VG_CLASS_DECL(PhysicsObjectComponent, IPhysicsObjectComponent);

        PhysicsObjectComponent(const core::string & _name, IObject * _parent);
        ~PhysicsObjectComponent();

        void                            SetCategory             (physics::Category _category) override;

        void                            EnableCollisionMask     (bool _enable) override;
        void                            SetCollisionMask        (physics::CategoryFlag _mask) override;

        VG_INLINE physics::Category     getCategory             () const;
        VG_INLINE bool                  isCollisionMaskEnabled  () const;
        VG_INLINE physics::CategoryFlag getCollisionMask        () const;

    private:
        physics::Category               m_category              = (physics::Category)0;

        bool                            m_useCollisionMask      = false;
        physics::CategoryFlag           m_collisionMask         = (physics::CategoryFlag)0;
    };
}

#if VG_ENABLE_INLINE
#include "PhysicsObjectComponent.inl"
#endif