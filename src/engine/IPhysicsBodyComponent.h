#pragma once

#include "engine/Component/Physics/Object/PhysicsObjectComponent.h"
#include "physics/Physics_Consts.h"

namespace vg::engine
{
    class IPhysicsBodyComponent : public PhysicsObjectComponent
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IPhysicsBodyComponent, PhysicsObjectComponent);

        virtual void                    SetMatrix(const core::float4x4 & _matrix) = 0;
        virtual void                    AddImpulse(const core::float3 & _impulse) = 0;

        virtual void                    SetTrigger(bool _trigger) = 0;
        virtual bool                    IsTrigger() const = 0;
    
        virtual void                    SetMotionType(physics::MotionType _motionType) = 0;
        virtual physics::MotionType     GetMotionType() const = 0;
    };
}