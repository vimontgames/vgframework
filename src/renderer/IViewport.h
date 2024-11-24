#pragma once

#include "gfx/IFrameGraphViewport.h"

namespace vg::renderer
{
    class IUIRenderer;

    struct CreateViewportParams
    {
        CreateViewportParams()
        {
        }

        CreateViewportParams(gfx::ViewportTarget _target, core::uint2 _size, gfx::ITexture * _dest = nullptr) :
            target(_target),
            size(_size),
            dest(_dest)
        {

        }
        gfx::ViewportTarget target = gfx::ViewportTarget::Game;
        core::uint2         size = core::uint2(0, 0);
        gfx::ITexture *     dest = nullptr;
    };

    enum class ViewportFlags : core::u32
    {

    };

    class IViewport : public gfx::IFrameGraphViewport
    {
    public:
        virtual void                                        SetRenderTargetSize (core::uint2 _size) = 0;
        virtual core::uint2                                 GetRenderTargetSize () const = 0;   

        virtual void                                        SetRenderTarget     (gfx::ITexture * _renderTarget) = 0;
        virtual gfx::ITexture *                             GetRenderTarget     () const = 0;

        virtual void                                        SetFlags            (ViewportFlags _flagsToSet, ViewportFlags _flagsToRemove = (ViewportFlags)0x0) = 0;
        virtual ViewportFlags                               GetFlags            () const = 0;

        virtual void                                        SetViewportID       (gfx::ViewportID _viewportID) = 0;
        virtual gfx::ViewportID                             GetViewportID       () const = 0;

        virtual void                                        SetFocused          (bool _active) = 0;
        virtual bool                                        AnyFocused          () const = 0;

        virtual void                                        SetVisible          (bool _visible) = 0;
        virtual bool                                        AnyVisible          () const = 0;

        virtual void                                        SetRender           (bool _visible) = 0;
        virtual bool                                        AnyRender           () const = 0;

        virtual void                                        AddView             (core::u8 _index, gfx::ViewID _viewID) = 0;
        virtual void                                        RemoveView          (core::u8 _index, gfx::ViewID _viewID) = 0;

        virtual const core::map<core::u8, gfx::ViewID> &    GetViewIDs          () const = 0;
        virtual IUIRenderer *                               GetUIRenderer       () const = 0;

        virtual const core::string                          GetFrameGraphID     (const core::string & _name) const = 0;
    };
}