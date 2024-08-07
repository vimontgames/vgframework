#pragma once

namespace vg::engine
{
    class IEngine;
}

namespace vg::physics
{
    struct PhysicsCreationParams
    {
        engine::IEngine * engine = nullptr;
        core::uint maxBodies = 65535;
        core::uint numBodyMutexes = 0;
	    core::uint maxBodyPairs = 65536;
	    core::uint maxContactConstraints = 10240;
        core::uint maxPhysicsBarriers = 8;
    };

    // Minimum dimension for physics shape
    const float shapeEps = 0.0001f;

    // Layer that objects can be in, determines which other objects it can collide with
    // Typically you at least want to have 1 layer for moving bodies and 1 layer for static bodies, but you can have more
    // layers if you want. E.g. you could have a layer for high detail collision (which is not used by the physics simulation
    // but only if you do collision testing).
    enum class ObjectLayer : core::u16
    {
        NonMoving = 0,
        Moving
    };

    // Each broadphase layer results in a separate bounding volume tree in the broad phase. You at least want to have
    // a layer for non-moving and moving objects to avoid having to update a tree full of static objects every frame.
    // You can have a 1-on-1 mapping between object layers and broadphase layers (like in this case) but if you have
    // many object layers you'll be creating many broad phase trees, which is not efficient. If you want to fine tune
    // your broadphase layers define JPH_TRACK_BROADPHASE_STATS and look at the stats reported on the TTY.
    enum class BPLayer : core::u8
    {
        NonMoving = 0,
        Moving
    };

    enum class MotionType : core::u8
    {
        Static = 0, // Non movable
        Kinematic,  // Movable using velocities only, does not respond to forces
        Dynamic,    // Responds to forces as a normal physics object
    };

    enum class ShapeType : core::u8
    {
        Sphere = 0,
        Box,
        Capsule,
        Cylinder,
        //ConvexHull,
        //TriangleMesh,
        //HeightField
    };

    enum class ShapeTypeFlags : core::u32
    {
        Sphere          = 1 << (core::u32)ShapeType::Sphere,
        Box             = 1 << (core::u32)ShapeType::Box,
        Capsule         = 1 << (core::u32)ShapeType::Capsule,
        Cylinder        = 1 << (core::u32)ShapeType::Cylinder,
        //ConvexHull      = 1 << (core::u32)ShapeType::ConvexHull,
        //TriangleMesh    = 1 << (core::u32)ShapeType::TriangleMesh,
        //HeightField     = 1 << (core::u32)ShapeType::HeightField
    };

    enum class BodyType : core::u8
    {
        Rigid = 0,
        Soft
    };

    enum class CharacterType : core::u8
    {
        Rigid = 0,
        Virtual
    };

    enum class GroundState : core::u8
    {
        Grounded,   // Character is on the ground 
        Blocked,    // Character is on a slope that is too steep and can't climb
        Touching,   // Character is touching an object, but is not supported and should fall
        InAir,      // Character is in the air not touching anything.
    };
}