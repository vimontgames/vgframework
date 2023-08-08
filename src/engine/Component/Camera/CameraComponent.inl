namespace vg::engine
{
    //--------------------------------------------------------------------------------------
    VG_INLINE void CameraComponent::setViewTarget(gfx::ViewTarget _target)
    {
        m_target = _target;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE gfx::ViewTarget CameraComponent::getViewTarget() const
    {
        return m_target;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE gfx::ViewID CameraComponent::getViewID() const
    {
        return gfx::ViewID(m_target, m_index);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE float CameraComponent::getFovY() const 
    { 
        return m_fovY; 
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE float CameraComponent::getNear() const 
    {
        return m_near; 
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE float CameraComponent::getFar() const 
    {
        return m_far; 
    }
}