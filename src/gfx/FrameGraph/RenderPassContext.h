#pragma once

#include "gfx/IView.h"
#include "gfx/IViewport.h"

namespace vg::gfx
{
    class IView;

    class RenderPassContext
    {
    public:     
        const core::string          getFrameGraphID     (const core::string & _name) const;
        const core::string          getFrameGraphIDEx   (const core::string & _name) const;

        static const core::string   MakeFrameGraphID    (const core::string & _name, ViewID _viewID);
        static const core::string   MakeFrameGraphIDEx  (const core::string & _name, ViewID _viewID, ViewID _parentViewID);
        static const core::string   MakeFrameGraphID    (const core::string & _name, ViewportID _viewportID);

        VG_INLINE const IView *     getView             () const;
        VG_INLINE IView *           getViewMutable      () const;
        VG_INLINE void              setView             (const IView * _view);


        VG_INLINE const IView *     getParentView       () const;
        VG_INLINE void              setParentView       (const IView * _parentView);

    private:
        IView * m_view = nullptr;
        IView * m_parent = nullptr;
    };
}

#if VG_ENABLE_INLINE
#include "RenderPassContext.inl"
#endif