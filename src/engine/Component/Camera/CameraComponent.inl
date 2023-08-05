namespace vg::engine
{
    //--------------------------------------------------------------------------------------
    VG_INLINE void CameraComponent::setViewType(gfx::ViewType _viewType)
    {
        m_ViewType = _viewType;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE gfx::ViewType CameraComponent::getViewType() const
    {
        return m_ViewType;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE gfx::ViewID CameraComponent::getViewID() const
    {
        return gfx::ViewID(m_ViewType, m_ViewIndex);
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