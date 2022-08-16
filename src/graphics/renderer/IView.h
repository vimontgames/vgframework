#pragma once

#include "core/Object/Object.h"

namespace vg::core
{
    class IGameObject;
}

namespace vg::graphics::renderer
{
    struct CreateViewParams
    {
        core::int2 offset;
        core::uint2 size;
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

        virtual void                    SetCameraSector     (core::IGameObject * _cameraSector) = 0;
        virtual core::IGameObject *     GetCameraSector     () const = 0;
    };
}