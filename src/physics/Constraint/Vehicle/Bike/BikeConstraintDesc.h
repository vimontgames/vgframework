#pragma once

#include "physics/Constraint/Vehicle/VehicleConstraintDesc.h"

namespace vg::core
{
    class GameObject;
}

namespace vg::physics
{
    //--------------------------------------------------------------------------------------
    class BikeConstraintDesc : public VehicleConstraintDesc
    {
    public:
        VG_CLASS_DECL(BikeConstraintDesc, VehicleConstraintDesc);

        BikeConstraintDesc(const vg::core::string & _name = "", vg::core::IObject * _parent = nullptr);
        ~BikeConstraintDesc();

        VehicleType GetVehicleType() const final override { return VehicleType::Bike; }
        core::uint GetWheelCount() const final override { return 2; }

        void OnLoad() final override;
        bool RegisterUID() final override;

        const core::GameObject * getHandlebar() const;
        const core::GameObject * getFork() const;

    public:
        // Misc
        float                   m_maxPitchRollAngleInDegrees = 180.0f;
        //float                   m_maxLeanAngleInDegrees = 35.0f;
        //float                   m_stabilizationSpeed = 5.0f;        // m/s
        //float                   m_lowSpeedAngularDamping = 0.90f;   // 0..1
        core::ObjectHandle      m_handlebar;
        core::ObjectHandle      m_fork;
        OneWheeledAxleDesc      m_front;
        OneWheeledAxleDesc      m_rear;
    };
}