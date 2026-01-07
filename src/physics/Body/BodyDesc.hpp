#include "BodyDesc.h"

using namespace vg::core;

namespace vg::physics
{
    VG_REGISTER_OBJECT_CLASS(PhysicsBodyDesc, "Body Settings");

    //--------------------------------------------------------------------------------------
    bool isStaticMergeHidden(const IObject * _object, const IProperty * _prop, uint _index)
    {
        const PhysicsBodyDesc * desc = VG_SAFE_STATIC_CAST(const PhysicsBodyDesc, _object);
        return desc->GetMotionType() != MotionType::Static;
    }

    //--------------------------------------------------------------------------------------
    bool PhysicsBodyDesc::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerProperty(PhysicsBodyDesc, m_trigger, "Trigger");
        setPropertyDescription(PhysicsBodyDesc, m_trigger, "Detects overlaps without affecting physics or collisions");

        registerProperty(PhysicsBodyDesc, m_softBody, "SoftBody");
        setPropertyDescription(PhysicsBodyDesc, m_softBody, "A flexible object that deforms and reacts to forces, simulating realistic bending and stretching");

        registerPropertyEnumEx(PhysicsBodyDesc, ObjectLayer, m_layer, "Layer", PropertyFlags::None);
        setPropertyDescription(PhysicsBodyDesc, m_layer, "Defines the object's interaction with other objects based on its movement status\n\nNon-Moving:\nThe object does not change position and typically remains stationary in the simulation\nMoving:\nThe object can change position and interact with other moving objects based on physics");

        registerPropertyEnumEx(PhysicsBodyDesc, MotionType, m_motion, "Motion", PropertyFlags::None);
        setPropertyDescription(PhysicsBodyDesc, m_motion, "Determines how an object moves and interacts with forces\n\nStatic:\nFixed in place, does not move or respond to forces\nKinematic:\nMoves under its own control, not affected by forces\nDynamic:\nAffected by forces and collisions, interacts physically");
        
        registerProperty(PhysicsBodyDesc, m_staticMerge, "Static Merge");
        setPropertyDescription(PhysicsBodyDesc, m_staticMerge, "This body can be merged with other static bodies at engine init");
        setPropertyHiddenCallback(PhysicsOptions, m_staticMerge, isStaticMergeHidden);

        registerPropertyEnumEx(PhysicsBodyDesc, MotionQuality, m_motionQuality, "Quality", PropertyFlags::None);
        setPropertyDescription(PhysicsBodyDesc, m_motionQuality, "Determines how accurately a physics engine calculates an object's motion and collision detection\nDiscrete:\nCollision checks occur only at specific time steps.\nContinuous:\nCollision checks are performed continuously along the object's movement path.");

        registerOptionalProperty(PhysicsBodyDesc, m_overrideMass, m_mass, "Mass");
        setPropertyDescription(PhysicsBodyDesc, m_mass, "Represents the amount of matter in an object, influencing its resistance to acceleration and the force required to move it");

        registerOptionalProperty(PhysicsBodyDesc, m_overrideCenterOfMassOffset, m_centerOfMassOffset, "COM Offset");
        setPropertyDescription(PhysicsBodyDesc, m_overrideCenterOfMassOffset, "Center Of Mass offset");
 
        registerProperty(PhysicsBodyDesc, m_friction, "Friction");
        setPropertyRange(PhysicsBodyDesc, m_friction, float2(0, 1));
        setPropertyDescription(PhysicsBodyDesc, m_friction, "Determines the resistance to sliding motion between surfaces, affecting how easily objects can slide over each other");

        registerProperty(PhysicsBodyDesc, m_restitution, "Restitution");
        setPropertyRange(PhysicsBodyDesc, m_restitution, float2(0, 1));
        setPropertyDescription(PhysicsBodyDesc, m_restitution, "Controls the bounciness of objects during collisions, ranging from 0 (no bounce) to 1 (full bounce)");

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool PhysicsBodyDesc::CanStaticMerge() const 
    {
        return GetMotionType() == physics::MotionType::Static && m_staticMerge; 
    }
}