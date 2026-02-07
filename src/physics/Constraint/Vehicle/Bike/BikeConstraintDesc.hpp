#include "BikeConstraintDesc.h"

using namespace vg::core;

namespace vg::physics
{
    VG_REGISTER_CLASS(BikeConstraintDesc, "Bike Constraint");

    //--------------------------------------------------------------------------------------
    // BikeConstraintDesc
    //--------------------------------------------------------------------------------------
    bool BikeConstraintDesc::registerProperties(IClassDesc & _desc)
    {
        //registerPropertyGroupBegin(BikeConstraintDesc, "Bike");
        {
            //registerProperty(BikeConstraintDesc, m_maxLeanAngleInDegrees, "MaxLeanAngle");
            //registerProperty(BikeConstraintDesc, m_stabilizationSpeed, "Stabilization Speed");
            //registerProperty(BikeConstraintDesc, m_lowSpeedAngularDamping, "Low Speed Angular Damping");

            // Misc
            registerProperty(BikeConstraintDesc, m_maxPitchRollAngleInDegrees, "Max Pitch Roll");
            setPropertyDescription(BikeConstraintDesc, m_maxPitchRollAngleInDegrees, "Defines the maximum pitch/roll angle (rad), can be used to avoid the car from getting upside down. The vehicle up direction will stay within a cone centered around the up axis with half top angle mMaxPitchRollAngle.")
            setPropertyRange(BikeConstraintDesc, m_maxPitchRollAngleInDegrees, float2(0, 180));

            //registerProperty(BikeConstraintDesc, m_fork, "Fork");
        }
        //registerPropertyGroupEnd(BikeConstraintDesc);

        // Wheels
        registerPropertyGroupBegin(BikeConstraintDesc, "Front");
        {
            registerPropertyObject(BikeConstraintDesc, m_front, "Front Wheels");
        }
        registerPropertyGroupEnd(BikeConstraintDesc);

        registerPropertyGroupBegin(BikeConstraintDesc, "Rear");
        {
            registerPropertyObject(BikeConstraintDesc, m_rear, "Rear Wheels");
        }
        registerPropertyGroupEnd(BikeConstraintDesc);

        super::registerProperties(_desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    BikeConstraintDesc::BikeConstraintDesc(const vg::core::string & _name, vg::core::IObject * _parent) :
        super(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    BikeConstraintDesc::~BikeConstraintDesc()
    {

    }

    //--------------------------------------------------------------------------------------
    void BikeConstraintDesc::OnLoad()
    {
        super::OnLoad();

        RegisterUID();
    }

    //--------------------------------------------------------------------------------------
    bool BikeConstraintDesc::RegisterUID()
    {
        super::RegisterUID();

        m_front.RegisterUID();
        m_rear.RegisterUID();

        return HasValidUID();
    }
}