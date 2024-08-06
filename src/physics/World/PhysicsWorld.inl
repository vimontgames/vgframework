namespace vg::physics
{
    //--------------------------------------------------------------------------------------
    VG_INLINE const vg::core::IWorld * PhysicsWorld::getWorld() const
    {
        return m_world;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE JPH::PhysicsSystem * PhysicsWorld::getPhysicsSystem() const
    {
        return m_physicsSystem;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE JPH::BodyInterface & PhysicsWorld::getBodyInterface() const
    {
        return m_physicsSystem->GetBodyInterface(); 
    }
}