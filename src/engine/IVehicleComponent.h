#pragma once

#include "engine/Component/Physics/Constraint/PhysicsConstraintComponent.h"

namespace vg::engine
{
    vg_enum_class(vg::engine, VehicleSlotType, core::u8,
        Driver = 0,
        Passenger = 1
    );

    class IVehicleComponent : public PhysicsConstraintComponent
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IVehicleComponent, PhysicsConstraintComponent);

        virtual bool                EnterVehicle            (core::IGameObject * _owner, VehicleSlotType & _slotType) = 0;
        virtual bool                ExitVehicle             (core::IGameObject * _owner) = 0;

        virtual void                Respawn                 () = 0;

        virtual core::uint          GetPassengerSlotCount   () const = 0;
        virtual core::IGameObject * GetPassenger            (core::uint _index) const = 0;
        virtual VehicleSlotType     GetPassengerSlotType    (core::uint _index) const = 0;

        virtual core::float3        GetLocalVelocity        () const = 0;

        virtual void                Accelerate              (float _forward) = 0;
        virtual void                Brake                   (float _brake) = 0;
        virtual void                Steer                   (float _leftRight) = 0;
    };
}