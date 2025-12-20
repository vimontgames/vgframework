#include "VehicleConstraintDesc.h"
#include "editor/Editor_Consts.h"

using namespace vg::core;

namespace vg::physics
{
    VG_REGISTER_OBJECT_CLASS(VehicleWheelDesc, "Vehicle Wheel Settings");
    VG_REGISTER_OBJECT_CLASS(VehicleWheelDescList, "Vehicle Wheel Settings List");
    VG_REGISTER_OBJECT_CLASS(VehicleConstraintDesc, "Vehicle Settings");

    //--------------------------------------------------------------------------------------
    // VehicleWheelDesc
    //--------------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------------
    void * ResizeVehicleWheelDescVector(IObject * _parent, uint _offset, uint _count)
    {
        auto vec = (core::vector<VehicleWheelDesc> *)(uint_ptr(_parent) + _offset);
        vec->clear();
        vec->resize(_count);
        return vec->data();
    }

    //--------------------------------------------------------------------------------------
    bool VehicleWheelDesc::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        setPropertyFlag(VehicleWheelDesc, m_name, PropertyFlags::Hidden, false);
        registerResizeVectorFunc(VehicleWheelDesc, ResizeVehicleWheelDescVector);

        registerProperty(VehicleWheelDesc, m_object, "Object");

        registerPropertyEnum(VehicleWheelDesc, VehicleWheelAxle, m_axle, "Axle");
        registerPropertyEnum(VehicleWheelDesc, VehicleWheelSide, m_side, "Side");

        registerProperty(VehicleWheelDesc, m_radius, "Radius");
        setPropertyDescription(VehicleWheelDesc, m_radius, "Wheel radius")
        setPropertyRange(VehicleWheelDesc, m_radius, float2(0,1));

        registerProperty(VehicleWheelDesc, m_width, "Width");
        setPropertyDescription(VehicleWheelDesc, m_width, "Wheel width")
        setPropertyRange(VehicleWheelDesc, m_width, float2(0, 1));

        return true;
    }

    //--------------------------------------------------------------------------------------
    // VehicleWheelDescList
    //--------------------------------------------------------------------------------------
    bool VehicleWheelDescList::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);
        return true;
    }

    //--------------------------------------------------------------------------------------
    // VehicleConstraintDesc
    //--------------------------------------------------------------------------------------
    bool VehicleConstraintDesc::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerProperty(VehicleConstraintDesc, m_fourWheelDrive, "4x4");

        registerProperty(VehicleConstraintDesc, m_maxPitchRollAngleInDegrees, "Max Pitch Roll");
        setPropertyDescription(VehicleConstraintDesc, m_maxPitchRollAngleInDegrees, "Defines the maximum pitch/roll angle (rad), can be used to avoid the car from getting upside down. The vehicle up direction will stay within a cone centered around the up axis with half top angle mMaxPitchRollAngle.")
        setPropertyRange(VehicleWheelDesc, m_maxPitchRollAngleInDegrees, float2(0, 180));

        registerPropertyObject(VehicleConstraintDesc, m_wheelDescList, editor::style::label::physics::Wheels);

        return true;
    }

    //--------------------------------------------------------------------------------------
    VehicleConstraintDesc::VehicleConstraintDesc(const vg::core::string & _name, vg::core::IObject * _parent) :
        super(_name, _parent),
        m_wheelDescList(_name, this)
    {
        
    }

    //--------------------------------------------------------------------------------------
    VehicleConstraintDesc::~VehicleConstraintDesc()
    {

    }

    //--------------------------------------------------------------------------------------
    void VehicleConstraintDesc::OnLoad()
    {
        RegisterUID();
        m_wheelDescList.RegisterUID();

        for (uint i = 0; i < m_wheelDescList.Size(); ++i)
            m_wheelDescList.getObjects()[i].RegisterUID();
    }
}