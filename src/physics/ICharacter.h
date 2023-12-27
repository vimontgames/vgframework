#pragma once

#include "core/Object/Object.h"
#include "Physics_Consts.h"

namespace vg::physics
{
    class ICharacterDesc : public core::Object
    {
    public:
        VG_CLASS_DECL_ABSTRACT(ICharacterDesc, core::Object)

        virtual CharacterType   GetCharacterType() const = 0;

        virtual void            SetMass         (float _mass) = 0;
        virtual void            SetMaxSlopeAngle(float _maxSlopeAngle) = 0;
        //virtual void            SetFriction     (float _friction) = 0;
    };

    class ICharacter : public core::Object
    {
    public:
        VG_CLASS_DECL_ABSTRACT(ICharacter, core::Object)

        virtual void            Activate        (const core::float4x4 & _world) = 0;
        virtual void            Deactivate      (const core::float4x4 & _world) = 0;

        virtual void            FixedUpdate     () = 0;
        virtual void            Update          () = 0;

        virtual core::float3    GetVelocity     () const = 0;
        virtual void            SetVelocity     (const core::float3 & _velocity) = 0;

        virtual void            SetPosition     (const core::float3 & _position) = 0;
        virtual void            SetRotation     (const core::quaternion & _rotation) = 0;

        virtual core::float4x4  GetMatrix       () const = 0;

        virtual GroundState     GetGroundState  () const = 0;
    };
}