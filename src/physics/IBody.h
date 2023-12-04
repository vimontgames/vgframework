#pragma once

#include "core/Object/Object.h"

namespace vg::physics
{
    class IBody : public core::Object
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IBody, core::Object)

        virtual void Activate(const core::float4x4 & _world) = 0;
        virtual void Deactivate(const core::float4x4 & _world) = 0;

        virtual core::float4x4 GetMatrix() const = 0;
    };
}