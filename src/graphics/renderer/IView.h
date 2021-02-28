#pragma once

#include "core/Object/Object.h"

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

        virtual void setViewInvMatrix(const core::float4x4 & _viewInv) = 0;
        virtual const core::float4x4 & getViewInvMatrix() const = 0;

    private:
    };
}