#include "physics/Precomp.h"
#include "Shape.h"
#include "Jolt/Physics/Collision/Shape/Shape.h"
#include "Jolt/Physics/Collision/Shape/SphereShape.h"
#include "Jolt/Physics/Collision/Shape/BoxShape.h"

#include "renderer/IDebugDraw.h"

#include "ShapeDesc.hpp"

namespace vg::physics
{
    //--------------------------------------------------------------------------------------
    Shape::Shape(const core::string & _name, core::IObject * _parent) :
        IShape(_name, _parent)
    {
    }

    //--------------------------------------------------------------------------------------
    Shape::~Shape()
    {
        m_shape->Release();
    }

    //--------------------------------------------------------------------------------------
    bool Shape::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);
        return true;
    }

    //--------------------------------------------------------------------------------------
    const core::float4x4 & Shape::GetTransform() const
    {
        return m_transform;
    }

    //--------------------------------------------------------------------------------------
    float Shape::GetMass() const
    {
        return m_shape->GetMassProperties().mMass;
    }

    //--------------------------------------------------------------------------------------
    renderer::IDebugDraw * Shape::getDebugDraw() const
    {
        return Physics::get()->getDebugDraw();
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
        m_shape = new JPH::SphereShape(m_radius);
        m_transform[3].xyz = _desc.m_offset;
        m_shape->AddRef();
    }

    //--------------------------------------------------------------------------------------
    void SphereShape::Draw(const float4x4 & _world)
    {
        float4x4 world = mul(m_transform, _world);
        const float radius = m_radius;
        getDebugDraw()->AddWireframeSphere(radius, 0xFF0000FF, world);
    }

    //--------------------------------------------------------------------------------------
    VG_REGISTER_OBJECT_CLASS(BoxShape, "Box Shape");

    //--------------------------------------------------------------------------------------
    bool BoxShape::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);
        return true;
    }

    //--------------------------------------------------------------------------------------
    BoxShape::BoxShape(const BoxShapeDesc & _desc) :
        Shape("Box", nullptr),
        m_size(_desc.m_size)
    {
        JPH::Vec3 halfSize = JPH::Vec3(m_size.x * 0.5f, m_size.y * 0.5f, m_size.z * 0.5f);
        m_shape = new JPH::BoxShape(halfSize);
        m_transform[3].xyz = _desc.m_offset;
        m_shape->AddRef();
    }

    //--------------------------------------------------------------------------------------
    void BoxShape::Draw(const float4x4 & _world)
    {
        float4x4 world = mul(m_transform, _world);
        const float3 halfSize = m_size * 0.5f;
        getDebugDraw()->AddWireframeBox(-halfSize, halfSize, 0xFF0000FF, world);
    }
}