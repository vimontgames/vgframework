#pragma once

#include "physics/IVehicleConstraint.h"

namespace vg::core
{
    class IWorld;
}

namespace JPH
{
    class VehicleCollisionTesterRay;
    class VehicleConstraint;
}

namespace vg::physics
{
    class VehicleConstraintDesc;
    class Shape;
    class PhysicsWorld;
    class Body;

    class VehicleConstraint final : public IVehicleConstraint
    {
    public:
        VG_CLASS_DECL(VehicleConstraint, IVehicleConstraint);

        VehicleConstraint(Body * _body, const VehicleConstraintDesc * _vehicleConstraintDesc, const core::string & _name, core::IObject * _parent);
        ~VehicleConstraint();

        void            FixedUpdate     (DriveState _driveState) final override;
        void            Update          (DriveState _driveState) final override;

        float           GetEngineRPM    () const final override;
        core::uint      GetCurrentGear  () const final override;

        core::float4x4  GetWheelMatrix  (core::uint _index) const final override;
        void            Reset           () final override;

    private:
        VehicleConstraintDesc *                 m_vehicleConstraintDesc         = nullptr;
        Body *                                  m_vehicleBody                   = nullptr;
        DriveState                              m_smoothedDriveState;
        core::IWorld *                          m_world                         = nullptr;
        JPH::PhysicsSystem *                    m_joltPhysicsSystem             = nullptr;
        JPH::VehicleCollisionTesterRay *        m_joltVehicleCollisionTester    = nullptr;
        JPH::VehicleConstraint *                m_joltVehicleConstraint         = nullptr;
    };    
}