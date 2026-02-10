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

    public:
        float                   m_maxPitchRollAngleInDegrees = 0.0f;
        core::ObjectHandle      m_handlebar;
        OneWheeledAxleDesc      m_front;
        OneWheeledAxleDesc      m_rear;
    };
}