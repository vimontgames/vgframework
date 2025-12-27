#pragma once

#include "physics/IVehicleConstraint.h"
#include "core/Object/ObjectList.h"
#include "core/Object/ObjectHandle.h"

namespace vg::physics
{
    //--------------------------------------------------------------------------------------
    class VehicleAxleDesc : public core::Object
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(VehicleAxleDesc, core::Object);

    public:
        // Wheels
        core::ObjectHandle  m_leftWheel;
        core::ObjectHandle  m_rightWheel;
        float               m_radius                    = 0.35f;
        float               m_width                     = 0.2f;
        
        // Suspension
        float               m_suspensionForwardAngleInDegree    = 0.0f;
        float               m_suspensionSidewaysAngleInDegree   = 0.0f;
        float               m_suspensionMinLength               = 0.3f;
        float               m_suspensionMaxLength               = 0.6f;
        float               m_suspensionFrequency               = 1.5f;
        float               m_suspensionDamping                 = 0.5f;

        // Brakes
        float               m_maxBrakeTorque            = 2000.0f;  
        float               m_maxHandBrakeTorque        = 4000.0f;  
        float               m_maxSteerAngleInDegrees    = 35.0f;   

        float               m_inertia                   = 0.9f;     
        float               m_angularDamping            = 0.2f;     
    };

    //--------------------------------------------------------------------------------------
    class VehicleConstraintDesc : public IVehicleConstraintDesc
    {
    public:
        VG_CLASS_DECL(VehicleConstraintDesc, IVehicleConstraintDesc);

        VehicleConstraintDesc(const vg::core::string & _name = "", vg::core::IObject * _parent = nullptr);
        ~VehicleConstraintDesc();

        virtual VehicleType GetVehicleType() const = 0;
        virtual core::uint GetWheelCount() const = 0;
    };

    //--------------------------------------------------------------------------------------
    class FourWheelsVehicleConstraintDesc : public VehicleConstraintDesc
    {
    public:
        VG_CLASS_DECL(FourWheelsVehicleConstraintDesc, VehicleConstraintDesc);

        FourWheelsVehicleConstraintDesc(const vg::core::string & _name = "", vg::core::IObject * _parent = nullptr);
        ~FourWheelsVehicleConstraintDesc();

        virtual VehicleType GetVehicleType() const final override { return VehicleType::FourWheels; }
        core::uint GetWheelCount() const final override { return 4; }

        void OnLoad() final override;

    public:
        // Engine
        float           m_maxEngineTorque               = 500.0f;
        float           m_minRPM                        = 1000.0f;
        float           m_maxRPM                        = 6000.0f;
        float           m_clutchStrength                = 10.0f;

        // Wheels
        bool            m_fourWheelDrive                = false;       
        VehicleAxleDesc m_front;
        VehicleAxleDesc m_rear;

        // Misc
        float           m_maxPitchRollAngleInDegrees    = 180.0f;
    };
}