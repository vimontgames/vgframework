namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    VG_INLINE const IFrameGraphView * RenderPassContext::getView() const
    {
        VG_ASSERT(m_views.size() == 1);
        return m_views[0];
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE IFrameGraphView * RenderPassContext::getViewMutable() const
    {
        VG_ASSERT(m_views.size() == 1);
        return m_views[0];
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void RenderPassContext::setView(const IFrameGraphView * _view)
    {
        m_views.resize(1);
        m_views[0] = const_cast<IFrameGraphView *>(_view);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE const core::vector<IFrameGraphView *> & RenderPassContext::getViews() const
    {
        return m_views;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void RenderPassContext::setViews(core::vector<IFrameGraphView *> & _views)
    {
        m_views = _views;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE const IFrameGraphView * RenderPassContext::getParentView() const
    {
        return m_parent;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void RenderPassContext::setParentView(const IFrameGraphView * _parentView)
    {
        m_parent = const_cast<IFrameGraphView *>(_parentView);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE const core::IWorld * RenderPassContext::getWorld() const
    {
        return m_world;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void RenderPassContext::setWorld(const core::IWorld * _world)
    {
        m_world = _world;
    }
}