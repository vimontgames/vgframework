namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    VG_INLINE const core::float4x4 & View::getViewInvMatrix() const
    {
        return m_viewInv;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE core::float2 View::getCameraNearFar() const
    {
        return m_cameraNearFar;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE float View::getCameraFovY() const
    {
        return m_cameraFovY;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE core::GameObject * View::getCameraSector() const
    {
        return m_cameraSector;
    }
}