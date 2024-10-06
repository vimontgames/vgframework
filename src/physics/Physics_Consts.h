#pragma once

namespace vg::engine
{
    class IEngine;
}

namespace vg::physics
{
    struct PhysicsCreationParams
    {
        engine::IEngine *           engine                  = nullptr;
        core::uint                  maxBodies               = 65535;
        core::uint                  numBodyMutexes          = 0;
	    core::uint                  maxBodyPairs            = 65536;
	    core::uint                  maxContactConstraints   = 10240;
        core::uint                  maxPhysicsBarriers      = 8;
    };

    // Minimum dimension for physics shape
    const float shapeEps = 0.0001f;

    // Layer that objects can be in, determines which other objects it can collide with
    // Typically you at least want to have 1 layer for moving bodies and 1 layer for static bodies, but you can have more
    // layers if you want. E.g. you could have a layer for high detail collision (which is not used by the physics simulation
    // but only if you do collision testing).
    vg_enum_class(ObjectLayer, core::u16,
        NonMoving = 0,
        Moving
    );

    // Each broadphase layer results in a separate bounding volume tree in the broad phase. You at least want to have
    // a layer for non-moving and moving objects to avoid having to update a tree full of static objects every frame.
    // You can have a 1-on-1 mapping between object layers and broadphase layers (like in this case) but if you have
    // many object layers you'll be creating many broad phase trees, which is not efficient. If you want to fine tune
    // your broadphase layers define JPH_TRACK_BROADPHASE_STATS and look at the stats reported on the TTY.
    vg_enum_class(BPLayer, core::u8,
        NonMoving = 0,
        Moving
    );

    vg_enum_class(MotionType, core::u8,
        Static = 0, // Non movable
        Kinematic,  // Movable using velocities only, does not respond to forces
        Dynamic     // Responds to forces as a normal physics object
    );

    vg_enum_class(MotionQuality, core::u8, 
        Discrete = 0,   // Update the body in discrete steps
        Continuous      // Update the body using linear casting
    );

    vg_enum_class(ShapeType, core::u8,
        Sphere = 0,
        Box,
        Capsule,
        Cylinder
    );

    vg_enum_class(ShapeTypeFlags, core::u32,
        Sphere          = 1 << (core::u32)ShapeType::Sphere,
        Box             = 1 << (core::u32)ShapeType::Box,
        Capsule         = 1 << (core::u32)ShapeType::Capsule,
        Cylinder        = 1 << (core::u32)ShapeType::Cylinder
    );

    vg_enum_class(BodyType, core::u8,
        Rigid = 0,
        Soft
    );

    vg_enum_class(CharacterType, core::u8,
        Rigid = 0,
        Virtual
    );

    vg_enum_class(GroundState, core::u8,
        Grounded,   // Character is on the ground 
        Blocked,    // Character is on a slope that is too steep and can't climb
        Touching,   // Character is touching an object, but is not supported and should fall
        InAir       // Character is in the air not touching anything.
    );

    class IShape;

    struct ShapeInfo
    {
        IShape *            m_shape = nullptr;
        core::float3        m_translation = core::float3(0, 0, 0);
        core::quaternion    m_rotation = core::quaternion(0, 0, 0, 1);
    };

    vg_enum_class(Category, core::u8,
        Category_0,  Category_1,  Category_2,  Category_3,  
        Category_4,  Category_5,  Category_6,  Category_7,
        Category_8,  Category_9,  Category_10, Category_11, 
        Category_12, Category_13, Category_14, Category_15,
        Category_16, Category_17, Category_18, Category_19, 
        Category_20, Category_21, Category_22, Category_23,
        Category_24, Category_25, Category_26, Category_27, 
        Category_28, Category_29, Category_30, Category_31,
        Category_32, Category_33, Category_34, Category_35, 
        Category_36, Category_37, Category_38, Category_39,
        Category_40, Category_41, Category_42, Category_43, 
        Category_44, Category_45, Category_46, Category_47,
        Category_48, Category_49, Category_50, Category_51, 
        Category_52, Category_53, Category_54, Category_55,
        Category_56, Category_57, Category_58, Category_59, 
        Category_60, Category_61, Category_62, Category_63
    );

    vg_enum_class(CategoryFlag, core::u64,
        CategoryFlag_0  = 1ULL << (core::u64)Category::Category_0,  CategoryFlag_1  = 1ULL << (core::u64)Category::Category_1,  CategoryFlag_2  = 1ULL << (core::u64)Category::Category_2,  CategoryFlag_3  = 1ULL << (core::u64)Category::Category_3,
        CategoryFlag_4  = 1ULL << (core::u64)Category::Category_4,  CategoryFlag_5  = 1ULL << (core::u64)Category::Category_5,  CategoryFlag_6  = 1ULL << (core::u64)Category::Category_6,  CategoryFlag_7  = 1ULL << (core::u64)Category::Category_7,
        CategoryFlag_8  = 1ULL << (core::u64)Category::Category_8,  CategoryFlag_9  = 1ULL << (core::u64)Category::Category_9,  CategoryFlag_10 = 1ULL << (core::u64)Category::Category_10, CategoryFlag_11 = 1ULL << (core::u64)Category::Category_11,
        CategoryFlag_12 = 1ULL << (core::u64)Category::Category_12, CategoryFlag_13 = 1ULL << (core::u64)Category::Category_13, CategoryFlag_14 = 1ULL << (core::u64)Category::Category_14, CategoryFlag_15 = 1ULL << (core::u64)Category::Category_15,
        CategoryFlag_16 = 1ULL << (core::u64)Category::Category_16, CategoryFlag_17 = 1ULL << (core::u64)Category::Category_17, CategoryFlag_18 = 1ULL << (core::u64)Category::Category_18, CategoryFlag_19 = 1ULL << (core::u64)Category::Category_19,
        CategoryFlag_20 = 1ULL << (core::u64)Category::Category_20, CategoryFlag_21 = 1ULL << (core::u64)Category::Category_21, CategoryFlag_22 = 1ULL << (core::u64)Category::Category_22, CategoryFlag_23 = 1ULL << (core::u64)Category::Category_23,
        CategoryFlag_24 = 1ULL << (core::u64)Category::Category_24, CategoryFlag_25 = 1ULL << (core::u64)Category::Category_25, CategoryFlag_26 = 1ULL << (core::u64)Category::Category_26, CategoryFlag_27 = 1ULL << (core::u64)Category::Category_27,
        CategoryFlag_28 = 1ULL << (core::u64)Category::Category_28, CategoryFlag_29 = 1ULL << (core::u64)Category::Category_29, CategoryFlag_30 = 1ULL << (core::u64)Category::Category_30, CategoryFlag_31 = 1ULL << (core::u64)Category::Category_31,
        CategoryFlag_32 = 1ULL << (core::u64)Category::Category_32, CategoryFlag_33 = 1ULL << (core::u64)Category::Category_33, CategoryFlag_34 = 1ULL << (core::u64)Category::Category_34, CategoryFlag_35 = 1ULL << (core::u64)Category::Category_35,
        CategoryFlag_36 = 1ULL << (core::u64)Category::Category_36, CategoryFlag_37 = 1ULL << (core::u64)Category::Category_37, CategoryFlag_38 = 1ULL << (core::u64)Category::Category_38, CategoryFlag_39 = 1ULL << (core::u64)Category::Category_39,
        CategoryFlag_40 = 1ULL << (core::u64)Category::Category_40, CategoryFlag_41 = 1ULL << (core::u64)Category::Category_41, CategoryFlag_42 = 1ULL << (core::u64)Category::Category_42, CategoryFlag_43 = 1ULL << (core::u64)Category::Category_43,
        CategoryFlag_44 = 1ULL << (core::u64)Category::Category_44, CategoryFlag_45 = 1ULL << (core::u64)Category::Category_45, CategoryFlag_46 = 1ULL << (core::u64)Category::Category_46, CategoryFlag_47 = 1ULL << (core::u64)Category::Category_47,
        CategoryFlag_48 = 1ULL << (core::u64)Category::Category_48, CategoryFlag_49 = 1ULL << (core::u64)Category::Category_49, CategoryFlag_50 = 1ULL << (core::u64)Category::Category_50, CategoryFlag_51 = 1ULL << (core::u64)Category::Category_51,
        CategoryFlag_52 = 1ULL << (core::u64)Category::Category_52, CategoryFlag_53 = 1ULL << (core::u64)Category::Category_53, CategoryFlag_54 = 1ULL << (core::u64)Category::Category_54, CategoryFlag_55 = 1ULL << (core::u64)Category::Category_55,
        CategoryFlag_56 = 1ULL << (core::u64)Category::Category_56, CategoryFlag_57 = 1ULL << (core::u64)Category::Category_57, CategoryFlag_58 = 1ULL << (core::u64)Category::Category_58, CategoryFlag_59 = 1ULL << (core::u64)Category::Category_59,
        CategoryFlag_60 = 1ULL << (core::u64)Category::Category_60, CategoryFlag_61 = 1ULL << (core::u64)Category::Category_61, CategoryFlag_62 = 1ULL << (core::u64)Category::Category_62, CategoryFlag_63 = 1ULL << (core::u64)Category::Category_63
    );
}