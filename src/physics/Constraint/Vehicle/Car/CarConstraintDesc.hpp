#include "CarConstraintDesc.h"

using namespace vg::core;

namespace vg::physics
{
    VG_REGISTER_CLASS(CarConstraintDesc, "Car Constraint");

    //--------------------------------------------------------------------------------------
    // CarConstraintDesc
    //--------------------------------------------------------------------------------------
    bool CarConstraintDesc::registerProperties(IClassDesc & _desc)
    {
        //registerPropertyGroupBegin(BikeConstraintDesc, "Car Settings");
        {
            registerProperty(CarConstraintDesc, m_fourWheelDrive, "Four-Wheel Drive");

            registerProperty(CarConstraintDesc, m_maxPitchRollAngleInDegrees, "Max Pitch Roll");
            setPropertyDescription(CarConstraintDesc, m_maxPitchRollAngleInDegrees, "Defines the maximum pitch/roll angle (rad), can be used to avoid the car from getting upside down. The vehicle up direction will stay within a cone centered around the up axis with half top angle mMaxPitchRollAngle.")
            setPropertyRange(CarConstraintDesc, m_maxPitchRollAngleInDegrees, float2(0, 180));
        }
        //registerPropertyGroupEnd(CarConstraintDesc);

        // Wheels
        registerPropertyGroupBegin(CarConstraintDesc, "Front");
        {
            registerPropertyObject(CarConstraintDesc, m_front, "Front Wheels");
        }
        registerPropertyGroupEnd(CarConstraintDesc);

        registerPropertyGroupBegin(CarConstraintDesc, "Rear");
        {
            registerPropertyObject(CarConstraintDesc, m_rear, "Rear Wheels");
        }
        registerPropertyGroupEnd(CarConstraintDesc);

        super::registerProperties(_desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    CarConstraintDesc::CarConstraintDesc(const vg::core::string & _name, vg::core::IObject * _parent) :
        super(_name, _parent)
    {
        m_front.m_maxHandBrakeTorque = 0.0f;
        m_rear.m_maxSteerAngleInDegrees = 0.0f;
    }

    //--------------------------------------------------------------------------------------
    CarConstraintDesc::~CarConstraintDesc()
    {

    }

    //--------------------------------------------------------------------------------------
    void CarConstraintDesc::OnLoad()
    {
        super::OnLoad();

        RegisterUID();
    }

    //--------------------------------------------------------------------------------------
    bool CarConstraintDesc::RegisterUID()
    {
        super::RegisterUID();

        m_front.RegisterUID();
        m_rear.RegisterUID();

        return HasValidUID();
    }
}