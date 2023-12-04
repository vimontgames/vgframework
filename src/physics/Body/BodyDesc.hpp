#include "BodyDesc.h"

using namespace vg::core;

namespace vg::physics
{
    //--------------------------------------------------------------------------------------
    bool BodyDesc::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        _desc.registerPropertyEnumEx(BodyDesc, ObjectLayer, m_layer, "Layer", IProperty::Flags::ReadOnly);
        _desc.registerPropertyEnumEx(BodyDesc, MotionType, m_motion, "Motion", IProperty::Flags::ReadOnly);

        return true;
    }

    //--------------------------------------------------------------------------------------
    VG_REGISTER_OBJECT_CLASS(RigidBodyDesc, "RigidBody Settings");
    //--------------------------------------------------------------------------------------
    bool RigidBodyDesc::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        // TODO : _desc.registerOptionalProperty(RigidBodyComponent, m_overrideMass, m_mass, "Mass");
        _desc.registerPropertyEx(RigidBodyDesc, m_overrideMass, "Override Mass", IProperty::Flags::Hidden);
        _desc.registerPropertyEx(RigidBodyDesc, m_mass, "Mass", IProperty::Flags::Optional);

        _desc.registerPropertyEnum(RigidBodyDesc, ShapeType, m_shapeType, "Shape");

        return true;
    }
}