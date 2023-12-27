#pragma once

#include "core/Component/Component.h"

namespace vg::engine
{
    class ICharacterControllerComponent : public core::Component
    {
    public:
        VG_CLASS_DECL_ABSTRACT(ICharacterControllerComponent, core::Component);

        virtual physics::GroundState    GetGroundState  () const = 0;

        virtual core::float3            GetVelocity     () const = 0;
        virtual void                    SetVelocity     (const core::float3 & _velocity) = 0;

        virtual void                    SetPosition     (const core::float3 & _position) = 0;
        virtual void                    SetRotation     (const core::quaternion & _rotation) = 0;
    };
}