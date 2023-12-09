#include "SphereShape.h"
#include "Jolt/Physics/Collision/Shape/SphereShape.h"

namespace vg::physics
{
    //--------------------------------------------------------------------------------------
    VG_REGISTER_OBJECT_CLASS(SphereShapeDesc, "Sphere Shape Settings");
    //--------------------------------------------------------------------------------------
    bool SphereShapeDesc::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);
        registerProperty(SphereShapeDesc, m_radius, "Radius");
        return true;
    }

    //--------------------------------------------------------------------------------------
    VG_REGISTER_OBJECT_CLASS(SphereShape, "Sphere Shape");
    //--------------------------------------------------------------------------------------
    bool SphereShape::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);
        return true;
    }

    //--------------------------------------------------------------------------------------
    SphereShape::SphereShape(const SphereShapeDesc & _desc) :
        Shape("Sphere", nullptr),
        m_radius(_desc.m_radius)
    {
        auto shape = new JPH::SphereShape(m_radius);
        m_shape = new JPH::RotatedTranslatedShape(JPH::Vec3(_desc.m_offset.x, _desc.m_offset.y, _desc.m_offset.z), JPH::Quat::sIdentity(), shape);
        m_transform[3].xyz = _desc.m_offset;
        m_shape->AddRef();
    }

    //--------------------------------------------------------------------------------------
    void SphereShape::Draw(const core::float4x4 & _world)
    {
        core::float4x4 world = mul(m_transform, _world);
        const float radius = m_radius;
        getDebugDraw()->AddWireframeSphere(radius, 0xFF0000FF, world);
    }
}
