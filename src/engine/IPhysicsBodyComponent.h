#pragma once

#include "core/Component/Component.h"

namespace vg::engine
{
    class IPhysicsBodyComponent : public core::Component
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IPhysicsBodyComponent, core::Component);

        virtual void SetMatrix(const core::float4x4 & _matrix) = 0;
        virtual void AddImpulse(const core::float3 & _impulse) = 0;

        virtual void SetTrigger(bool _trigger) = 0;
        virtual bool IsTrigger() const = 0;
    };
}