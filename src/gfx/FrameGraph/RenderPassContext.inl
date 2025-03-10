namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    VG_INLINE const IFrameGraphView * RenderPassContext::getView() const
    {
        return m_view;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE IFrameGraphView * RenderPassContext::getViewMutable() const
    {
        return m_view;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void RenderPassContext::setView(const IFrameGraphView * _view)
    {
        m_view = const_cast<IFrameGraphView*>(_view);
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