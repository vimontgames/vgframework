#pragma once

#include "engine/Component/Physics/Object/PhysicsObjectComponent.h"

namespace vg::engine
{
    class ICharacterControllerComponent : public PhysicsObjectComponent
    {
    public:
        VG_CLASS_DECL_ABSTRACT(ICharacterControllerComponent, PhysicsObjectComponent);

        virtual physics::GroundState    GetGroundState  () const = 0;

        virtual core::float3            GetVelocity     () const = 0;
        virtual void                    SetVelocity     (const core::float3 & _velocity) = 0;

        virtual void                    SetPosition     (const core::float3 & _position) = 0;
        virtual void                    SetRotation     (const core::quaternion & _rotation) = 0;

        virtual bool                    CanMoveTo       (const core::float3 & _position) = 0;
    };
}