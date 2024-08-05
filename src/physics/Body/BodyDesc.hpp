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
        setPropertyDescription(PhysicsBodyDesc, m_trigger, "Detects overlaps without affecting physics or collisions");

        registerProperty(PhysicsBodyDesc, m_softBody, "SoftBody");
        setPropertyDescription(PhysicsBodyDesc, m_softBody, "A flexible object that deforms and reacts to forces, simulating realistic bending and stretching");

        registerPropertyEnumEx(PhysicsBodyDesc, ObjectLayer, m_layer, "Layer", IProperty::Flags::ReadOnly);
        setPropertyDescription(PhysicsBodyDesc, m_layer, "Defines the object's interaction with other objects based on its movement status\n\nNon-Moving: The object does not change position and typically remains stationary in the simulation\nMoving: The object can change position and interact with other moving objects based on physics");

        registerPropertyEnumEx(PhysicsBodyDesc, MotionType, m_motion, "Motion", IProperty::Flags::ReadOnly);
        setPropertyDescription(PhysicsBodyDesc, m_motion, "Determines how an object moves and interacts with forces\nStatic: Fixed in place, does not move or respond to forces\n\nKinematic: Moves under its own control, not affected by forces\nDynamic: Affected by forces and collisions, interacts physically");
        
        registerOptionalProperty(PhysicsBodyDesc, m_overrideMass, m_mass, "Mass");
        setPropertyDescription(PhysicsBodyDesc, m_mass, "Represents the amount of matter in an object, influencing its resistance to acceleration and the force required to move it");

        registerProperty(PhysicsBodyDesc, m_friction, "Friction");
        setPropertyRange(PhysicsBodyDesc, m_friction, float2(0, 1));
        setPropertyDescription(PhysicsBodyDesc, m_friction, "Determines the resistance to sliding motion between surfaces, affecting how easily objects can slide over each other");

        registerProperty(PhysicsBodyDesc, m_restitution, "Restitution");
        setPropertyRange(PhysicsBodyDesc, m_restitution, float2(0, 1));
        setPropertyDescription(PhysicsBodyDesc, m_restitution, "Controls the bounciness of objects during collisions, ranging from 0 (no bounce) to 1 (full bounce)");

        return true;
    }
}