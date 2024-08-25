#pragma once

#include "core/Object/Object.h"
#include "gfx/IView.h"

namespace vg::gfx
{
    class ITexture;

    enum class ViewportTarget : core::u8
    {
        Game        = 0,
        Editor      = 1
    };
    static inline const ViewportTarget ViewportTargetInvalid = (ViewportTarget)-1;

    using ViewportIndex = core::u8;
    static inline const ViewportIndex ViewportIndexInvalid = (ViewportIndex)-1;

    struct ViewportID
    {
        ViewportID(ViewportTarget _target = ViewportTargetInvalid, ViewportIndex _index = ViewportIndexInvalid) :
            target(_target),
            index(_index)
        {

        }

        union
        {
            struct
            {
                ViewportTarget target : 2;
                ViewportIndex index : 6;
            };
            core::u8 id;
        };

        inline bool operator == (const ViewportID & _other) const
        {
            return target == _other.target && index == _other.index;
        }

        inline bool operator != (const ViewportID & _other) const
        {
            return !(*this == _other);
        }
    };

    struct CreateViewportParams
    {
        CreateViewportParams()
        {
        }

        CreateViewportParams(ViewportTarget _target, core::uint2 _size, gfx::ITexture * _dest = nullptr) :
            target(_target),
            size(_size),
            dest(_dest)
        {

        }
        ViewportTarget  target = ViewportTarget::Game;
        core::uint2     size = core::uint2(0, 0);
        gfx::ITexture * dest = nullptr;
    };

    enum class ViewportFlags : core::u32
    {

    };

    class IView;

    class IViewport : public core::Object
    {
    public:

        IViewport() {};
        virtual ~IViewport() = default;

        virtual void                                SetRenderTargetSize (core::uint2 _size) = 0;
        virtual core::uint2                         GetRenderTargetSize () const = 0;   

        virtual void                                SetRenderTarget     (gfx::ITexture * _renderTarget) = 0;
        virtual gfx::ITexture *                     GetRenderTarget     () const = 0;

        virtual void                                SetFlags            (ViewportFlags _flagsToSet, ViewportFlags _flagsToRemove = (ViewportFlags)0x0) = 0;
        virtual ViewportFlags                       GetFlags            () const = 0;

        virtual void                                SetViewportID       (ViewportID _viewportID) = 0;
        virtual gfx::ViewportID                     GetViewportID       () const = 0;

        virtual void                                SetFocused          (bool _active) = 0;
        virtual bool                                AnyFocused          () const = 0;

        virtual void                                SetVisible          (bool _visible) = 0;
        virtual bool                                AnyVisible          () const = 0;

        virtual void                                SetRender           (bool _visible) = 0;
        virtual bool                                AnyRender           () const = 0;

        virtual void                                AddView             (core::u8 _index, ViewID _viewID) = 0;
        virtual void                                RemoveView          (core::u8 _index, ViewID _viewID) = 0;

        virtual const core::map<core::u8, ViewID> & GetViewIDs          () const = 0;

        virtual const core::string                  GetFrameGraphID     (const core::string & _name) const = 0;
    };
}