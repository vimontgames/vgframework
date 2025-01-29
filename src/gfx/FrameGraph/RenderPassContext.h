#pragma once

#include "gfx/IFrameGraphView.h"
#include "gfx/IFrameGraphViewport.h"

namespace vg::gfx
{
    class IFrameGraphView;

    class RenderPassContext
    {
    public:     
        const core::string                  getFrameGraphID     (const core::string & _name) const;
        const core::string                  getFrameGraphIDEx   (const core::string & _name) const;

        static const core::string           MakeFrameGraphID    (const core::string & _name, ViewID _viewID);
        static const core::string           MakeFrameGraphIDEx  (const core::string & _name, ViewID _viewID, ViewID _parentViewID);
        static const core::string           MakeFrameGraphID    (const core::string & _name, ViewportID _viewportID);

        VG_INLINE const IFrameGraphView *   getView             () const;
        VG_INLINE IFrameGraphView *         getViewMutable      () const;
        VG_INLINE void                      setView             (const IFrameGraphView * _view);

        VG_INLINE const IFrameGraphView *   getParentView       () const;
        VG_INLINE void                      setParentView       (const IFrameGraphView * _parentView);

        VG_INLINE const core::IWorld *      getWorld            () const;
        VG_INLINE void                      setWorld            (const core::IWorld * _world);

    private:
        const core::IWorld *                m_world  = nullptr;
        IFrameGraphView *                   m_view   = nullptr;
        IFrameGraphView *                   m_parent = nullptr;
    };
}

#if VG_ENABLE_INLINE
#include "RenderPassContext.inl"
#endif