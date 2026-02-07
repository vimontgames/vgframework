#include "VehicleConstraintDesc.h"
#include "editor/Editor_Consts.h"

using namespace vg::core;

namespace vg::physics
{
    VG_REGISTER_ABSTRACT_CLASS(AxleDesc);
    VG_REGISTER_CLASS(OneWheeledAxleDesc, "One-Wheeled Axle Settings");
    VG_REGISTER_CLASS(TwoWheeledAxleDesc, "Two-Wheeled Axle Settings");
    
    VG_REGISTER_ABSTRACT_CLASS(VehicleConstraintDesc);

    //--------------------------------------------------------------------------------------
    // AxleDesc
    //--------------------------------------------------------------------------------------
    bool AxleDesc::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerProperty(AxleDesc, m_radius, "Radius");
        setPropertyDescription(AxleDesc, m_radius, "Wheel radius")
        setPropertyRange(AxleDesc, m_radius, float2(0, 1));

        registerProperty(AxleDesc, m_width, "Width");
        setPropertyDescription(AxleDesc, m_width, "Wheel width")
        setPropertyRange(AxleDesc, m_width, float2(0, 1));

        registerProperty(AxleDesc, m_maxSteerAngleInDegrees, "Max Steer Angle");
        setPropertyDescription(AxleDesc, m_maxSteerAngleInDegrees, "How much this wheel can steer")
        setPropertyRange(AxleDesc, m_maxSteerAngleInDegrees, float2(0, 90));

        registerProperty(AxleDesc, m_inertia, "Inertia");
        setPropertyDescription(AxleDesc, m_inertia, "Moment of inertia (kg m^2)")
        setPropertyRange(AxleDesc, m_inertia, float2(0, 1));

        registerProperty(AxleDesc, m_angularDamping, "Angular Damping");
        setPropertyDescription(AxleDesc, m_angularDamping, "Angular damping factor of the wheel")
        setPropertyRange(AxleDesc, m_angularDamping, float2(0, 1));

        // Suspension
        registerPropertyGroupBegin(AxleDesc, "Suspension");
        {
            registerProperty(AxleDesc, m_suspensionMinLength, "Min Length");
            setPropertyDescription(AxleDesc, m_suspensionMinLength, "How long the suspension is in min raised position relative to the attachment point (m)")
            setPropertyRange(AxleDesc, m_suspensionMinLength, float2(0, 1));

            registerProperty(AxleDesc, m_suspensionMaxLength, "Max Length");
            setPropertyDescription(AxleDesc, m_suspensionMaxLength, "How long the suspension is in max raised position relative to the attachment point (m)")
            setPropertyRange(AxleDesc, m_suspensionMaxLength, float2(0, 1));

            registerProperty(AxleDesc, m_suspensionForwardAngleInDegree, "Forward angle");
            setPropertyDescription(AxleDesc, m_suspensionForwardAngleInDegree, "Suspension forward angle in degrees")
            setPropertyRange(AxleDesc, m_suspensionForwardAngleInDegree, float2(-180, 180));

            registerProperty(AxleDesc, m_suspensionSidewaysAngleInDegree, "Sideways angle");
            setPropertyDescription(AxleDesc, m_suspensionSidewaysAngleInDegree, "Suspension sideways angle in degrees")
            setPropertyRange(AxleDesc, m_suspensionSidewaysAngleInDegree, float2(-180, 180));

            registerProperty(AxleDesc, m_suspensionFrequency, "Frequency");
            setPropertyDescription(AxleDesc, m_suspensionFrequency, "oscillation frequency in Hz")
            setPropertyRange(AxleDesc, m_suspensionFrequency, float2(0, 10));

            registerProperty(AxleDesc, m_suspensionDamping, "Damping");
            setPropertyDescription(AxleDesc, m_suspensionDamping, "Damping ratio")
            setPropertyRange(AxleDesc, m_suspensionDamping, float2(0, 1));
        }
        registerPropertyGroupEnd(AxleDesc);

        registerPropertyGroupBegin(AxleDesc, "Brakes");
        {
            registerProperty(AxleDesc, m_maxBrakeTorque, "Brake");
            setPropertyDescription(AxleDesc, m_maxBrakeTorque, "How much torque (Nm) the brakes can apply to this wheel")
            setPropertyRange(AxleDesc, m_maxBrakeTorque, float2(0, 10000));

            registerProperty(AxleDesc, m_maxHandBrakeTorque, "HandBrake");
            setPropertyDescription(AxleDesc, m_maxHandBrakeTorque, "How much torque (Nm) the hand brake can apply to this wheel (usually only applied to the rear wheels)")
            setPropertyRange(AxleDesc, m_maxHandBrakeTorque, float2(0, 10000));
        }
        registerPropertyGroupEnd(AxleDesc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    // OneWheeledAxleDesc
    //--------------------------------------------------------------------------------------
    bool OneWheeledAxleDesc::registerProperties(IClassDesc & _desc)
    {
        registerProperty(OneWheeledAxleDesc, m_wheel, "Wheel");

        super::registerProperties(_desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    // TwoWheeledAxleDesc
    //--------------------------------------------------------------------------------------
    bool TwoWheeledAxleDesc::registerProperties(IClassDesc & _desc)
    {
        registerProperty(TwoWheeledAxleDesc, m_leftWheel, "Left");
        registerProperty(TwoWheeledAxleDesc, m_rightWheel, "Right");

        super::registerProperties(_desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    // VehicleConstraintDesc
    //--------------------------------------------------------------------------------------
    bool VehicleConstraintDesc::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        // Engine
        registerPropertyGroupBegin(VehicleConstraintDesc, "Engine");
        {
            registerProperty(VehicleConstraintDesc, m_maxEngineTorque, "Engine");
            setPropertyDescription(VehicleConstraintDesc, m_maxEngineTorque, "Max amount of torque (Nm) that the engine can deliver.")
            setPropertyRange(VehicleConstraintDesc, m_maxEngineTorque, float2(0, 2000));

            registerProperty(VehicleConstraintDesc, m_minRPM, "Min RPM");
            setPropertyDescription(VehicleConstraintDesc, m_minRPM, "Min amount of revolutions per minute (rpm) the engine can produce without stalling.")
            setPropertyRange(VehicleConstraintDesc, m_minRPM, float2(1, 10000));

            registerProperty(VehicleConstraintDesc, m_maxRPM, "Max RPM");
            setPropertyDescription(VehicleConstraintDesc, m_maxRPM, "Max amount of revolutions per minute (rpm) the engine can generate.")
            setPropertyRange(VehicleConstraintDesc, m_maxRPM, float2(1, 10000));

            registerProperty(VehicleConstraintDesc, m_clutchStrength, "Clutch");
            setPropertyDescription(VehicleConstraintDesc, m_clutchStrength, "Strength of the clutch when fully engaged.")
            setPropertyRange(VehicleConstraintDesc, m_clutchStrength, float2(0, 100));
        }
        registerPropertyGroupEnd(VehicleConstraintDesc);

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
}