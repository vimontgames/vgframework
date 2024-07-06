#include "CapsuleShape.h"

namespace vg::physics
{
    //--------------------------------------------------------------------------------------
    VG_REGISTER_OBJECT_CLASS(CapsuleShapeDesc, "Capsule Shape Settings");
    //--------------------------------------------------------------------------------------
    bool CapsuleShapeDesc::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);
        registerProperty(CapsuleShapeDesc, m_radius, "Radius");
        setPropertyRange(CapsuleShapeDesc, m_radius, float2(0.01f, 100.0f));

        registerProperty(CapsuleShapeDesc, m_height, "Height");
        setPropertyRange(CapsuleShapeDesc, m_height, float2(0.01f, 100.0f));

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool CapsuleShapeDesc::TryGetAABB(AABB & _aabb) const
    {
        _aabb = AABB(-float3(m_height * 0.5f, m_height * 0.5f, m_height * 0.5f) + m_offset, float3(m_height * 0.5f, m_height * 0.5f, m_height * 0.5f) + m_offset);
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
        float height = m_height - 2.0f * m_radius;

        JPH::ConvexShape * shape = nullptr;

        #pragma push_macro("new")
        #undef new
        if (height > 0.0f)
            shape = new JPH::CapsuleShape(max(0.0f, m_height - 2.0f * m_radius) * 0.5f, m_radius);
        else
            shape = new JPH::SphereShape(m_radius);
        #pragma pop_macro("new")

        #pragma push_macro("new")
        #undef new
        m_shape = new JPH::RotatedTranslatedShape(JPH::Vec3(_desc.m_offset.x, _desc.m_offset.y, _desc.m_offset.z), JPH::Quat::sRotation(JPH::Vec3(1, 0, 0), PI / 2.0f), shape);
        #pragma pop_macro("new")

        m_transform[3].xyz = _desc.m_offset;
        m_shape->AddRef();
    }

    //--------------------------------------------------------------------------------------
    void CapsuleShape::Draw(const core::IWorld * _world, const core::float4x4 & _matrix)
    {
        float4x4 matrix = mul(m_transform, getMatrixWithoutScale(_matrix));
        getDebugDraw()->AddCapsule(_world, m_radius, m_height, 0xFF0000FF, matrix);
    }
}
