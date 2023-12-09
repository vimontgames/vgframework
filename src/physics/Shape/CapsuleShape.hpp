#include "CapsuleShape.h"
#include "Jolt/Physics/Collision/Shape/CapsuleShape.h"

namespace vg::physics
{
    //--------------------------------------------------------------------------------------
    VG_REGISTER_OBJECT_CLASS(CapsuleShapeDesc, "Capsule Shape Settings");
    //--------------------------------------------------------------------------------------
    bool CapsuleShapeDesc::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);
        registerProperty(CapsuleShapeDesc, m_radius, "Radius");
        registerProperty(CapsuleShapeDesc, m_height, "Height");
        return true;
    }

    //--------------------------------------------------------------------------------------
    VG_REGISTER_OBJECT_CLASS(CapsuleShape, "Capsule Shape");
    //--------------------------------------------------------------------------------------
    bool CapsuleShape::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);
        return true;
    }

    //--------------------------------------------------------------------------------------
    CapsuleShape::CapsuleShape(const CapsuleShapeDesc & _desc) :
        Shape("Capsule", nullptr),
        m_radius(_desc.m_radius),
        m_height(_desc.m_height)
    {
        auto shape = new JPH::CapsuleShape(max(0.0f, m_height - 2.0f * m_radius) * 0.5f, m_radius);
        m_shape = new JPH::RotatedTranslatedShape(JPH::Vec3(_desc.m_offset.x, _desc.m_offset.y, _desc.m_offset.z), JPH::Quat::sRotation(JPH::Vec3(1, 0, 0), PI / 2.0f), shape);
        m_transform[3].xyz = _desc.m_offset;
        m_shape->AddRef();
    }

    //--------------------------------------------------------------------------------------
    void CapsuleShape::Draw(const float4x4 & _world)
    {
        float4x4 world = mul(m_transform, _world);
        getDebugDraw()->AddCapsule(m_radius, m_height, 0xFF0000FF, world);
    }
}
