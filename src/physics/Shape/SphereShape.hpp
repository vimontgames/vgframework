#include "SphereShape.h"

using namespace vg::core;

namespace vg::physics
{
    //--------------------------------------------------------------------------------------
    VG_REGISTER_OBJECT_CLASS(SphereShapeDesc, "Sphere Shape Settings");
    //--------------------------------------------------------------------------------------
    bool SphereShapeDesc::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);
        registerProperty(SphereShapeDesc, m_radius, "Radius");
        setPropertyRange(SphereShapeDesc, m_radius, float2(0.01f, 100.0f));

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool SphereShapeDesc::TryGetAABB(AABB & _aabb) const
    {
        _aabb = AABB(float3(-m_radius, -m_radius, -m_radius) + m_offset, float3(+m_radius, +m_radius, +m_radius) + m_offset);
        return true;
    }

    //--------------------------------------------------------------------------------------
    VG_REGISTER_OBJECT_CLASS(SphereShape, "Sphere Shape");
    //--------------------------------------------------------------------------------------
    bool SphereShape::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);
        return true;
    }

    //--------------------------------------------------------------------------------------
    SphereShape::SphereShape(const SphereShapeDesc & _desc) :
        Shape("Sphere", nullptr),
        m_radius(_desc.m_radius)
    {
        #pragma push_macro("new")
        #undef new
        const float eps = 0.0001f;
        auto shape = new JPH::SphereShape(max(abs(m_radius), eps));
        #pragma pop_macro("new")

        #pragma push_macro("new")
        #undef new
        m_shape = new JPH::RotatedTranslatedShape(JPH::Vec3(_desc.m_offset.x, _desc.m_offset.y, _desc.m_offset.z), JPH::Quat::sIdentity(), shape);
        #pragma pop_macro("new")

        m_transform[3].xyz = _desc.m_offset;
        m_shape->AddRef();
    }

    //--------------------------------------------------------------------------------------
    void SphereShape::Draw(const core::IWorld * _world, const core::float4x4 & _matrix)
    {
        core::float4x4 matrix = mul(m_transform, getMatrixWithoutScale(_matrix));
        const float radius = m_radius;
        getDebugDraw()->AddWireframeSphere(_world, radius, m_color, matrix);
    }
}
