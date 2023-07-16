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

    //--------------------------------------------------------------------------------------
    VG_INLINE core::IUniverse * View::getUniverse() const
    {
        return m_cameraUniverse;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void View::setSize(core::uint2 _size)
    {
        m_size = _size;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE core::uint2 View::getSize() const
    {
        return m_size;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void View::setOffset(core::int2 _offset)
    {
        m_offset = _offset;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE core::int2 View::getOffset() const
    {
        return m_offset;
    }
}