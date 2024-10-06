namespace vg::engine
{
    //--------------------------------------------------------------------------------------
    VG_INLINE physics::Category PhysicsObjectComponent::getCategory() const
    {
        return m_category;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE bool PhysicsObjectComponent::isCollisionMaskEnabled() const
    {
        return m_useCollisionMask;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE physics::CategoryFlag PhysicsObjectComponent::getCollisionMask() const
    {
        return m_useCollisionMask ? m_collisionMask : (physics::CategoryFlag)0xFF;
    }
}