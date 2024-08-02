#include "BodyDesc.h"

using namespace vg::core;

namespace vg::physics
{
    VG_REGISTER_OBJECT_CLASS(PhysicsBodyDesc, "Body Settings");

    //--------------------------------------------------------------------------------------
    bool PhysicsBodyDesc::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerProperty(PhysicsBodyDesc, m_trigger, "Trigger");
        registerPropertyEx(PhysicsBodyDesc, m_softBody, "SoftBody", IProperty::Flags::SameLine);

        registerPropertyEnumEx(PhysicsBodyDesc, ObjectLayer, m_layer, "Layer", IProperty::Flags::ReadOnly);
        registerPropertyEnumEx(PhysicsBodyDesc, MotionType, m_motion, "Motion", IProperty::Flags::ReadOnly);

        registerOptionalProperty(PhysicsBodyDesc, m_overrideMass, m_mass, "Mass");

        registerProperty(PhysicsBodyDesc, m_friction, "Friction");
        setPropertyRange(PhysicsBodyDesc, m_friction, float2(0, 1));

        registerProperty(PhysicsBodyDesc, m_restitution, "Restitution");
        setPropertyRange(PhysicsBodyDesc, m_restitution, float2(0, 1));
     
        registerPropertyEnum(PhysicsBodyDesc, ShapeType, m_shapeType, "Shape");

        return true;
    }
}