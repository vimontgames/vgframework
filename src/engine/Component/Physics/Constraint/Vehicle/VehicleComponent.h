#pragma once

#include "engine/IVehicleComponent.h"
#include "core/Object/ObjectHandle.h"
#include "core/Object/ObjectList.h"
#include "physics/Physics_Consts.h"
#include "physics/IVehicleConstraint.h"

namespace vg::engine
{
    //--------------------------------------------------------------------------------------
    class VehicleSlot : public core::Object
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(VehicleSlot, core::Object);

    public:
        VehicleSlotType m_slotType = VehicleSlotType::Passenger;
        core::ObjectHandle m_owner;
    };

    //--------------------------------------------------------------------------------------
    class VehicleSlotList : public core::ObjectList<VehicleSlot>
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(VehicleSlotList, ObjectList<VehicleSlot>);
    };

    //--------------------------------------------------------------------------------------
    class VehicleComponent final : public IVehicleComponent
    {
    public:
        VG_CLASS_DECL(VehicleComponent, IVehicleComponent);

        VehicleComponent(const core::string & _name, IObject * _parent);
        ~VehicleComponent();

        void                OnPropertyChanged           (IObject * _object, const core::IProperty & _prop, bool _notifyParent);

        void                OnLoad                      ();
        void                OnPlay                      (); 
        void                OnStop                      ();

        void                FixedUpdate                 (const Context & _context) final override;
        void                Update                      (const Context & _context) final override;

        bool                EnterVehicle                (core::IGameObject * _owner, VehicleSlotType & _slotType) final override;
        bool                ExitVehicle                 (core::IGameObject * _owner) final override;

        void                Respawn                     () final override;

        core::uint          GetPassengerSlotCount       () const final override;
        core::IGameObject * GetPassenger                (core::uint _index) const final override;
        VehicleSlotType     GetPassengerSlotType        (core::uint _index) const final override;

        core::float3        GetLocalVelocity            () const final override;

        void                Accelerate                  (float _forward) final override;
        void                Brake                       (float _brake) final override;
        void                Steer                       (float _leftRight) final override;

    private:
        physics::IPhysics * getPhysics                  ();

        bool                createVehicleConstraintDesc ();
        bool                createVehicleConstraint     ();
        void                resetDriveState             ();

    private:
        physics::DriveState               m_driveState;
        core::float3                      m_localVelocity = (core::float3)0.0f;
        physics::IVehicleConstraintDesc * m_vehicleConstraintDesc = nullptr;
        physics::IVehicleConstraint *     m_vehicleConstraint = nullptr;
        VehicleSlotList                   m_slots;
        core::float4x4                    m_startPos = core::float4x4::identity();
    };
}