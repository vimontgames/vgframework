namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    VG_INLINE const core::float4x4 & View::getViewProjMatrix() const
    {
        return m_viewProj;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE const core::float4x4 & View::getViewMatrix() const
    {
        return m_view;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE const core::float4x4 & View::getViewInvMatrix() const
    {
        return m_viewInv;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE const core::float4x4 & View::getProjMatrix() const
    {
        return m_proj;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE const core::float4x4 & View::getProjInvMatrix() const
    {
        return m_projInv;
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
    VG_INLINE core::IWorld * View::getWorld() const
    {
        return m_camWorld;
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

    //--------------------------------------------------------------------------------------
    VG_INLINE void View::setViewID(gfx::ViewID _viewID)
    {
        m_viewID = _viewID;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE gfx::ViewID View::getViewID() const
    {
        return m_viewID;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void View::setRenderTarget(gfx::Texture * _renderTarget)
    {
        if (_renderTarget != m_renderTarget)
        {
            VG_SAFE_RELEASE(m_renderTarget);
            m_renderTarget = _renderTarget;
            VG_SAFE_INCREASE_REFCOUNT(m_renderTarget);
        }
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE gfx::Texture * View::getRenderTarget() const
    {
        return m_renderTarget;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE core::Job * View::getCullingJob() const
    {
        return m_cullingJob;
    }
}