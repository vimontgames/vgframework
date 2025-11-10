namespace vg::engine
{
    //--------------------------------------------------------------------------------------
    VG_INLINE void CameraComponent::setViewportTarget(gfx::ViewportTarget _target)
    {
        m_target = _target;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE gfx::ViewportTarget CameraComponent::getViewportTarget() const
    {
        return m_target;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE float CameraComponent::getFocalLength() const
    {
        return m_cameraSettings->GetFocalLength();
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE float CameraComponent::getNear() const 
    {
        return m_cameraSettings->GetNear();
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE float CameraComponent::getFar() const 
    {
        return m_cameraSettings->GetFar();
    }
}