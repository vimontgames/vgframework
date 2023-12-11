#include "BodyDesc.h"

using namespace vg::core;

namespace vg::physics
{
    //--------------------------------------------------------------------------------------
    bool BodyDesc::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyEnumEx(BodyDesc, ObjectLayer, m_layer, "Layer", IProperty::Flags::ReadOnly);
        registerPropertyEnumEx(BodyDesc, MotionType, m_motion, "Motion", IProperty::Flags::ReadOnly);

        registerOptionalProperty(RigidBodyDesc, m_overrideMass, m_mass, "Mass");

        registerProperty(BodyDesc, m_friction, "Friction");
        setPropertyRange(BodyDesc, m_friction, float2(0, 1));

        registerProperty(BodyDesc, m_restitution, "Restitution");
        setPropertyRange(BodyDesc, m_restitution, float2(0, 1));
     
        registerPropertyEnum(RigidBodyDesc, ShapeType, m_shapeType, "Shape");

        return true;
    }

    //--------------------------------------------------------------------------------------
    VG_REGISTER_OBJECT_CLASS(RigidBodyDesc, "RigidBody Settings");
    //--------------------------------------------------------------------------------------
    bool RigidBodyDesc::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }
}