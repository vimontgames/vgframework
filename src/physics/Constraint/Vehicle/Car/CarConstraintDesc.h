#pragma once

#include "physics/Constraint/Vehicle/VehicleConstraintDesc.h"

namespace vg::physics
{
    //--------------------------------------------------------------------------------------
    class CarConstraintDesc : public VehicleConstraintDesc
    {
    public:
        VG_CLASS_DECL(CarConstraintDesc, VehicleConstraintDesc);

        CarConstraintDesc(const vg::core::string & _name = "", vg::core::IObject * _parent = nullptr);
        ~CarConstraintDesc();

        VehicleType GetVehicleType() const final override { return VehicleType::Car; }
        core::uint GetWheelCount() const final override { return 4; }

        void OnLoad() final override;
        bool RegisterUID() final override;

    public:
        // Wheels
        bool                m_fourWheelDrive = false;
        TwoWheeledAxleDesc  m_front;
        TwoWheeledAxleDesc  m_rear;

        // Misc
        float           m_maxPitchRollAngleInDegrees = 180.0f;
    };
}