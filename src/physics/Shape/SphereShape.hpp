#include "SphereShape.h"

using namespace vg::core;

namespace vg::physics
{
    //--------------------------------------------------------------------------------------
    VG_REGISTER_CLASS(SphereShapeDesc, "Sphere Shape Settings");
    //--------------------------------------------------------------------------------------
    bool SphereShapeDesc::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerProperty(SphereShapeDesc, m_radius, "Radius");
        setPropertyRange(SphereShapeDesc, m_radius, float2(0.01f, 100.0f));
        setPropertyDescription(SphereShapeDesc, m_radius, "Sphere radius");

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool SphereShapeDesc::TryGetAABB(AABB & _aabb) const
    {
        _aabb = AABB(float3(-m_radius, -m_radius, -m_radius) + m_offset, float3(+m_radius, +m_radius, +m_radius) + m_offset);
        return true;
    }

    //--------------------------------------------------------------------------------------
    VG_REGISTER_CLASS(SphereShape, "Sphere Shape");
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
        auto shape = new JPH::SphereShape(max(abs(m_radius), shapeEps));
        #pragma pop_macro("new")

        #pragma push_macro("new")
        #undef new
        m_shape = new JPH::RotatedTranslatedShape(getJoltVec3(_desc.getTranslation()), getJoltQuaternion(_desc.getRotation()), shape);
        #pragma pop_macro("new")

        m_transform = _desc.getMatrix();
        m_shape->AddRef();
    }

    //--------------------------------------------------------------------------------------
    void SphereShape::Draw(const core::IWorld * _world, const core::float4x4 & _matrix) const
    {
        core::float4x4 matrix = mul(m_transform, clearScale(_matrix));
        const float radius = m_radius;
        getDebugDraw()->AddWireframeSphere(_world, radius, m_color, matrix);
    }
}
