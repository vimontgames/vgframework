#include "VehicleConstraintDesc.h"
#include "editor/Editor_Consts.h"

using namespace vg::core;

namespace vg::physics
{
    VG_REGISTER_CLASS(VehicleAxleDesc, "Vehicle Axle Settings");
    VG_REGISTER_ABSTRACT_CLASS(VehicleConstraintDesc);
    VG_REGISTER_CLASS(FourWheelsVehicleConstraintDesc, "Vehicle Settings");

    //--------------------------------------------------------------------------------------
    // VehicleAxleDesc
    //--------------------------------------------------------------------------------------
    bool VehicleAxleDesc::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerProperty(VehicleAxleDesc, m_leftWheel, "Left");
        registerProperty(VehicleAxleDesc, m_rightWheel, "Right");

        registerProperty(VehicleAxleDesc, m_radius, "Radius");
        setPropertyDescription(VehicleAxleDesc, m_radius, "Wheel radius")
        setPropertyRange(VehicleAxleDesc, m_radius, float2(0, 1));

        registerProperty(VehicleAxleDesc, m_width, "Width");
        setPropertyDescription(VehicleAxleDesc, m_width, "Wheel width")
        setPropertyRange(VehicleAxleDesc, m_width, float2(0, 1));

        registerProperty(VehicleAxleDesc, m_maxSteerAngleInDegrees, "Max Steer Angle");
        setPropertyDescription(VehicleAxleDesc, m_maxSteerAngleInDegrees, "How much this wheel can steer")
        setPropertyRange(VehicleAxleDesc, m_maxSteerAngleInDegrees, float2(0, 90));

        registerProperty(VehicleAxleDesc, m_inertia, "Inertia");
        setPropertyDescription(VehicleAxleDesc, m_inertia, "Moment of inertia (kg m^2)")
        setPropertyRange(VehicleAxleDesc, m_inertia, float2(0, 1));

        registerProperty(VehicleAxleDesc, m_angularDamping, "Angular Damping");
        setPropertyDescription(VehicleAxleDesc, m_angularDamping, "Angular damping factor of the wheel")
        setPropertyRange(VehicleAxleDesc, m_angularDamping, float2(0, 1));

        // Suspension
        registerPropertyGroupBegin(FourWheelsVehicleConstraintDesc, "Suspension");
        {
            registerProperty(VehicleAxleDesc, m_suspensionMinLength, "Min Length");
            setPropertyDescription(VehicleAxleDesc, m_suspensionMinLength, "How long the suspension is in min raised position relative to the attachment point (m)")
            setPropertyRange(VehicleAxleDesc, m_suspensionMinLength, float2(0, 1));

            registerProperty(VehicleAxleDesc, m_suspensionMaxLength, "Max Length");
            setPropertyDescription(VehicleAxleDesc, m_suspensionMaxLength, "How long the suspension is in max raised position relative to the attachment point (m)")
            setPropertyRange(VehicleAxleDesc, m_suspensionMaxLength, float2(0, 1));

            registerProperty(VehicleAxleDesc, m_suspensionForwardAngleInDegree, "Forward angle");
            setPropertyDescription(VehicleAxleDesc, m_suspensionForwardAngleInDegree, "Suspension forward angle in degrees")
            setPropertyRange(VehicleAxleDesc, m_suspensionForwardAngleInDegree, float2(-180, 180));

            registerProperty(VehicleAxleDesc, m_suspensionSidewaysAngleInDegree, "Sideways angle");
            setPropertyDescription(VehicleAxleDesc, m_suspensionSidewaysAngleInDegree, "Suspension sideways angle in degrees")
            setPropertyRange(VehicleAxleDesc, m_suspensionSidewaysAngleInDegree, float2(-180, 180));

            registerProperty(VehicleAxleDesc, m_suspensionFrequency, "Frequency");
            setPropertyDescription(VehicleAxleDesc, m_suspensionFrequency, "oscillation frequency in Hz")
            setPropertyRange(VehicleAxleDesc, m_suspensionFrequency, float2(0, 10));

            registerProperty(VehicleAxleDesc, m_suspensionDamping, "Damping");
            setPropertyDescription(VehicleAxleDesc, m_suspensionDamping, "Damping ratio")
            setPropertyRange(VehicleAxleDesc, m_suspensionDamping, float2(0, 1));
        }
        registerPropertyGroupEnd(FourWheelsVehicleConstraintDesc);

        registerPropertyGroupBegin(FourWheelsVehicleConstraintDesc, "Brakes");
        {
            registerProperty(VehicleAxleDesc, m_maxBrakeTorque, "Brake");
            setPropertyDescription(VehicleAxleDesc, m_maxBrakeTorque, "How much torque (Nm) the brakes can apply to this wheel")
            setPropertyRange(VehicleAxleDesc, m_maxBrakeTorque, float2(0, 10000));

            registerProperty(VehicleAxleDesc, m_maxHandBrakeTorque, "HandBrake");
            setPropertyDescription(VehicleAxleDesc, m_maxHandBrakeTorque, "How much torque (Nm) the hand brake can apply to this wheel (usually only applied to the rear wheels)")
            setPropertyRange(VehicleAxleDesc, m_maxHandBrakeTorque, float2(0, 10000));
        }
        registerPropertyGroupEnd(FourWheelsVehicleConstraintDesc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    // VehicleConstraintDesc
    //--------------------------------------------------------------------------------------
    bool VehicleConstraintDesc::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    VehicleConstraintDesc::VehicleConstraintDesc(const vg::core::string & _name, vg::core::IObject * _parent) :
        super(_name, _parent)
    {
        
    }

    //--------------------------------------------------------------------------------------
    VehicleConstraintDesc::~VehicleConstraintDesc()
    {

    }

    //--------------------------------------------------------------------------------------
    // FourWheelsVehicleConstraintDesc
    //--------------------------------------------------------------------------------------
    bool FourWheelsVehicleConstraintDesc::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        // Engine
        registerPropertyGroupBegin(FourWheelsVehicleConstraintDesc, "Engine");
        {
            registerProperty(FourWheelsVehicleConstraintDesc, m_maxEngineTorque, "Engine");
            setPropertyDescription(FourWheelsVehicleConstraintDesc, m_maxEngineTorque, "Max amount of torque (Nm) that the engine can deliver.")
            setPropertyRange(FourWheelsVehicleConstraintDesc, m_maxEngineTorque, float2(0, 2000));

            registerProperty(FourWheelsVehicleConstraintDesc, m_minRPM, "Min RPM");
            setPropertyDescription(FourWheelsVehicleConstraintDesc, m_minRPM, "Min amount of revolutions per minute (rpm) the engine can produce without stalling.")
            setPropertyRange(FourWheelsVehicleConstraintDesc, m_minRPM, float2(1, 10000));

            registerProperty(FourWheelsVehicleConstraintDesc, m_maxRPM, "Max RPM");
            setPropertyDescription(FourWheelsVehicleConstraintDesc, m_maxRPM, "Max amount of revolutions per minute (rpm) the engine can generate.")
            setPropertyRange(FourWheelsVehicleConstraintDesc, m_maxRPM, float2(1, 10000));

            registerProperty(FourWheelsVehicleConstraintDesc, m_clutchStrength, "Clutch");
            setPropertyDescription(FourWheelsVehicleConstraintDesc, m_clutchStrength, "Strength of the clutch when fully engaged.")
            setPropertyRange(FourWheelsVehicleConstraintDesc, m_clutchStrength, float2(0, 100));
        }
        registerPropertyGroupEnd(FourWheelsVehicleConstraintDesc);

        // Wheels
        registerPropertyGroupBegin(FourWheelsVehicleConstraintDesc, "Front Wheels");
        {
            registerPropertyObject(FourWheelsVehicleConstraintDesc, m_front, "Front Wheels");
        }
        registerPropertyGroupEnd(FourWheelsVehicleConstraintDesc);

        registerPropertyGroupBegin(FourWheelsVehicleConstraintDesc, "Rear Wheels");
        {
            registerPropertyObject(FourWheelsVehicleConstraintDesc, m_rear, "Rear Wheels");
        }
        registerPropertyGroupEnd(FourWheelsVehicleConstraintDesc);

        // Misc
        registerPropertyGroupBegin(FourWheelsVehicleConstraintDesc, "Misc");
        {
            registerProperty(FourWheelsVehicleConstraintDesc, m_fourWheelDrive, "Four-Wheel Drive");

            registerProperty(FourWheelsVehicleConstraintDesc, m_maxPitchRollAngleInDegrees, "Max Pitch Roll");
            setPropertyDescription(FourWheelsVehicleConstraintDesc, m_maxPitchRollAngleInDegrees, "Defines the maximum pitch/roll angle (rad), can be used to avoid the car from getting upside down. The vehicle up direction will stay within a cone centered around the up axis with half top angle mMaxPitchRollAngle.")
            setPropertyRange(FourWheelsVehicleConstraintDesc, m_maxPitchRollAngleInDegrees, float2(0, 180));
        }
        registerPropertyGroupEnd(FourWheelsVehicleConstraintDesc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    FourWheelsVehicleConstraintDesc::FourWheelsVehicleConstraintDesc(const vg::core::string & _name, vg::core::IObject * _parent) :
        super(_name, _parent)
    {
        m_front.m_maxHandBrakeTorque = 0.0f;
        m_rear.m_maxSteerAngleInDegrees = 0.0f;
    }

    //--------------------------------------------------------------------------------------
    FourWheelsVehicleConstraintDesc::~FourWheelsVehicleConstraintDesc()
    {

    }

    //--------------------------------------------------------------------------------------
    void FourWheelsVehicleConstraintDesc::OnLoad()
    {
        super::OnLoad();

        RegisterUID();

        m_front.RegisterUID();
        m_rear.RegisterUID();
    }
}