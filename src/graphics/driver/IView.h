#pragma once

#include "core/Object/Object.h"
#include "core/IUniverse.h"

namespace vg::core
{
    class IGameObject;
}

namespace vg::graphics::driver
{
    class ITexture;

    struct CreateViewParams
    {
        CreateViewParams()
        {
        }

        CreateViewParams(core::uint2 _size, core::int2 _offset = core::int2(0, 0), core::IUniverse * _universe = nullptr, driver::ITexture * _target = nullptr) :
            size(_size),
            offset(_offset),
            universe(_universe),
            target(_target)
        {

        }

        core::uint2 size = core::uint2(0, 0);
        core::int2 offset = core::int2(0, 0);
        core::IUniverse * universe = nullptr;
        driver::ITexture * target = nullptr;
    };

    using ViewID = core::u16;

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

        virtual void                    SetCameraSector     (core::IGameObject * _cameraSector) = 0;
        virtual core::IGameObject *     GetCameraSector     () const = 0;

        virtual void                    SetRenderTarget     (driver::ITexture * _renderTarget) = 0;
        virtual driver::ITexture *      GetRenderTarget     () const = 0;

        virtual void                    SetViewID           (ViewID _viewID) = 0;
        virtual ViewID                  GetViewID           () const = 0;
    };
}