namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    VG_INLINE const IView * RenderPassContext::getView() const
    {
        return m_view;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE IView * RenderPassContext::getViewMutable() const
    {
        return m_view;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void RenderPassContext::setView(const IView * _view)
    {
        m_view = const_cast<IView*>(_view);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE const IView * RenderPassContext::getParentView() const
    {
        return m_parent;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void RenderPassContext::setParentView(const IView * _parentView)
    {
        m_parent = const_cast<IView *>(_parentView);
    }
}