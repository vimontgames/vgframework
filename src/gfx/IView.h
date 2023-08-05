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

    enum class ViewType : core::u8
    {
        Backbuffer  = 0,
        Game        = 1,
        Editor      = 2
    };
    using ViewIndex = core::u8;

    struct ViewID
    {
        ViewID(ViewType _type, ViewIndex _index) :
            type(_type),
            index(_index)
        {
            
        }

        ViewType type   : 2;
        ViewIndex index : 6;

        inline bool operator == (const ViewID & _other) const
        {
            return type == _other.type && index == _other.index;
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

        CreateViewParams(ViewType _type, core::uint2 _size, core::int2 _offset = core::int2(0, 0), core::IUniverse * _universe = nullptr, gfx::ITexture * _target = nullptr) :
            type(_type),
            size(_size),
            offset(_offset),
            universe(_universe),
            target(_target)
        {
         
        }

        ViewType type = ViewType::Game;
        core::uint2 size = core::uint2(0, 0);
        core::int2 offset = core::int2(0, 0);
        core::IUniverse * universe = nullptr;
        gfx::ITexture * target = nullptr;
    };

    class IView : public core::Object
    {
    public:
        IView(const CreateViewParams & _params) {};
        virtual ~IView() = default;

        virtual void                    SetupCamera         (const core::float4x4 & _viewInv, core::float2 _nearFar, float _fovY) = 0;

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
        virtual gfx::ITexture *      GetRenderTarget     () const = 0;

        virtual void                    SetViewID           (ViewID _viewID) = 0;
        virtual ViewID                  GetViewID           () const = 0;

        virtual void                    SetActive           (bool _active) = 0;
        virtual bool                    IsActive            () const = 0;
    };
}