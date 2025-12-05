#include "RenderPassContext.h"

#if !VG_ENABLE_INLINE
#include "RenderPassContext.inl"
#endif

using namespace vg::core;

namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    const string RenderPassContext::getFrameGraphID(const string & _name) const
    {
        VG_ASSERT(m_views.size() == 1);
        return MakeFrameGraphID(_name, m_views[0]->GetViewID());
    }

    //--------------------------------------------------------------------------------------
    const string RenderPassContext::getFrameGraphIDEx(const string & _name) const
    {
        VG_ASSERT(m_views.size() == 1);
        return MakeFrameGraphIDEx(_name, m_views[0]->GetViewID(), m_parent->GetViewID());
    }

    //--------------------------------------------------------------------------------------
    const string RenderPassContext::MakeFrameGraphID(const string & _name, ViewID _viewID)
    {
        return fmt::sprintf("%s - %s %u", _name, asString(_viewID.target), _viewID.index);
    }

    //--------------------------------------------------------------------------------------
    const string RenderPassContext::MakeFrameGraphIDEx(const string & _name, ViewID _viewID, ViewID _parentViewID)
    {
        return fmt::sprintf("%s %u - %s %u", _name, _viewID.index, asString(_parentViewID.target), _parentViewID.index);
    }

    //--------------------------------------------------------------------------------------
    const string RenderPassContext::MakeFrameGraphID(const string & _name, ViewportID _viewportID)
    {
        return fmt::sprintf("%s - %s %u", _name, asString(_viewportID.target), _viewportID.index);
    }
}