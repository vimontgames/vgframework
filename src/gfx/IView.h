#pragma once

#include "core/Object/Object.h"
#include "core/IUniverse.h"

namespace vg::core
{
    class IGameObject;
}

namespace vg::gfx
{
    class ITexture;

    enum class ViewTarget : core::u8
    {
        Backbuffer  = 0,
        Game        = 1,
        Editor      = 2
    };
    static inline const ViewTarget ViewTargetInvalid = (ViewTarget)-1;

    using ViewIndex = core::u8;
    static inline const ViewIndex ViewIndexInvalid = (ViewIndex)-1;

    struct ViewID
    {
        ViewID(ViewTarget _target = ViewTargetInvalid, ViewIndex _index = ViewIndexInvalid) :
            target(_target),
            index(_index)
        {
            
        }

        ViewTarget target   : 2;
        ViewIndex index     : 6;

        inline bool operator == (const ViewID & _other) const
        {
            return target == _other.target && index == _other.index;
        }

        inline bool operator != (const ViewID & _other) const
        {
            return !(*this == _other);
        }
    };

    struct CreateViewParams
    {
        CreateViewParams()
        {
        }

        CreateViewParams(ViewTarget _target, core::uint2 _size, core::int2 _offset = core::int2(0, 0), core::IUniverse * _universe = nullptr, gfx::ITexture * _dest = nullptr) :
            target(_target),
            size(_size),
            offset(_offset),
            universe(_universe),
            dest(_dest)
        {
         
        }

        ViewTarget          target = ViewTarget::Game;
        core::uint2         size = core::uint2(0, 0);
        core::int2          offset = core::int2(0, 0);
        core::IUniverse *   universe = nullptr;
        gfx::ITexture *     dest = nullptr;
    };

    class IView : public core::Object
    {
    public:
        enum class Flags : core::u32
        {
            Picking = 0x00000001
        };

        IView(const CreateViewParams & _params) {};
        virtual ~IView() = default;

        virtual void                    SetupCamera         (const core::float4x4 & _viewInv, core::float2 _nearFar, float _fovY) = 0;

        virtual void                    SetFlags            (Flags _flagsToSet, Flags _flagsToRemove = (Flags)0) = 0;
        virtual Flags                   GetFlags            () const = 0;

        virtual const core::float4x4 &  GetViewInvMatrix    () const = 0;
        virtual core::float2            GetCameraNearFar    () const = 0;
        virtual float                   GetCameraFovY       () const = 0;

        virtual void                    SetUniverse         (core::IUniverse * _universe) = 0;
        virtual core::IUniverse *       GetUniverse         () const = 0;

        virtual void                    SetSize             (core::uint2 _size) = 0;
        virtual core::uint2             GetSize             () const = 0;

        virtual void                    SetOffset           (core::int2) = 0;
        virtual core::int2              GetOffset           () const = 0;

        virtual void                    SetRenderTarget     (gfx::ITexture * _renderTarget) = 0;
        virtual gfx::ITexture *         GetRenderTarget     () const = 0;

        virtual void                    SetViewID           (ViewID _viewID) = 0;
        virtual ViewID                  GetViewID           () const = 0;

        virtual void                    SetActive           (bool _active) = 0;
        virtual bool                    IsActive            () const = 0;

        virtual void                    SetMouseOffset      (const core::uint2 & _mouseOffset) = 0;
        virtual core::uint2             GetRelativeMousePos () const = 0;

        virtual const core::string      GetFrameGraphID     (const core::string & _name) const = 0;
        virtual bool                    IsToolmode          () const = 0;
    };
}