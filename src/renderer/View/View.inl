namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    VG_INLINE void View::setFlags(Flags _flagsToSet, Flags _flagsToRemove)
    {
        m_flags = (Flags)((std::underlying_type<Flags>::type(m_flags) & ~std::underlying_type<Flags>::type(_flagsToRemove)) | (std::underlying_type<Flags>::type(_flagsToSet)));
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void View::setFlag(Flags _flag, bool _value)
    {
        setFlags(_value ? _flag : (Flags)0x0, !_value ? _flag : (Flags)0x0);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE View::Flags View::getFlags() const
    {
        return m_flags;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE bool View::testFlag(Flags _flag) const
    {
        return asBool(_flag & getFlags());
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void View::setIsAdditionalView(bool _isAdditionalView) 
    { 
        setFlag(Flags::Additional, _isAdditionalView);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE bool View::isAdditionalView() const 
    { 
        return testFlag(Flags::Additional);
    }

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
    VG_INLINE const Frustum & View::getCameraFrustum() const 
    { 
        return m_frustum;
    };

    //--------------------------------------------------------------------------------------
    VG_INLINE void View::setViewID(gfx::ViewID _viewID)
    {
        VG_ASSERT(_viewID.index < 63);
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
    VG_INLINE core::float2 View::getViewportOffset() const
    {
        return saturate(m_viewportOffset);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE core::float2 View::getViewportScale() const
    {
        return saturate(m_viewportScale);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE ViewCullingJob * View::getCullingJob() const
    {
        return m_cullingJob;
    }
}