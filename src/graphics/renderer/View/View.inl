namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    VG_INLINE const core::float4x4 & View::getViewInvMatrix() const
    {
        return m_viewInv;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE core::Sector * View::getCameraSector() const
    {
        return m_cameraSector;
    }
}