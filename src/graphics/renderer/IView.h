#pragma once

#include "core/Object/Object.h"

namespace vg::core
{
    class ISector;
}

namespace vg::graphics::renderer
{
    struct CreateViewParams
    {
        core::int2 offset;
        core::uint2 size;
    };

    class IView : public core::ObjectPointer
    {
    public:
        IView(const CreateViewParams & _params) {};
        virtual ~IView() = default;

        virtual void SetViewInvMatrix(const core::float4x4 & _viewInv) = 0;
        virtual const core::float4x4 & GetViewInvMatrix() const = 0;

        virtual void SetCameraSector(core::ISector * _cameraSector) = 0;
        virtual core::ISector * GetCameraSector() const = 0;
    };
}