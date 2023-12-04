#include "physics/Precomp.h"
#include "Shape.h"
#include "Jolt/Physics/Collision/Shape/Shape.h"
#include "Jolt/Physics/Collision/Shape/SphereShape.h"
#include "Jolt/Physics/Collision/Shape/BoxShape.h"

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
    VG_REGISTER_OBJECT_CLASS(SphereShape, "Sphere Shape");

    //--------------------------------------------------------------------------------------
    bool SphereShape::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);
        return true;
    }

    //--------------------------------------------------------------------------------------
    SphereShape::SphereShape(const SphereShapeDesc & _desc) : 
        Shape("Sphere", nullptr)
    {
        m_shape = new JPH::SphereShape(_desc.m_radius);
        m_transform[3].xyz = _desc.m_offset;
        m_shape->AddRef();
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
        Shape("Box", nullptr)
    {
        JPH::Vec3 halfExtent = JPH::Vec3(_desc.m_size.x * 0.5f, _desc.m_size.y * 0.5f, _desc.m_size.z * 0.5f);
        m_shape = new JPH::BoxShape(halfExtent);
        m_transform[3].xyz = _desc.m_offset;
        m_shape->AddRef();
    }
}