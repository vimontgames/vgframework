namespace vg::engine
{
    //--------------------------------------------------------------------------------------
    VG_INLINE void CameraComponent::setViewType(graphics::driver::ViewType _viewType)
    {
        m_ViewType = _viewType;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE graphics::driver::ViewType CameraComponent::getViewType() const
    {
        return m_ViewType;
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