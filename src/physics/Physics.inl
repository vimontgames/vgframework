
namespace vg::physics
{
    //--------------------------------------------------------------------------------------
    VG_INLINE const PhysicsCreationParams & Physics::getPhysicsCreationParams() const
    {
        return m_physicsCreationParams; 
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE const Callbacks & Physics::getCallbacks() const
    {
        return m_callbacks;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE BroadPhaseLayer & Physics::getBroadPhaseLayer() 
    {
        return m_broadPhaseLayer; 
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE BroadPhaseFilter & Physics::getBroadPhaseFilter() 
    {
        return m_broadPhaseFilter; 
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE ObjectFilter & Physics::getObjectfilter() 
    {
        return m_objectFilter; 
    }
}

