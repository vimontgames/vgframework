#include "physics/Precomp.h"
#include "Shape.h"

#include "core/Math/Math.h"
#include "core/Misc/AABB/AABB.h"
#include "renderer/IDebugDraw.h"
#include "physics/Physics.h"

#include "SphereShape.hpp"
#include "BoxShape.hpp"
#include "CapsuleShape.hpp"

namespace vg::physics
{
    //--------------------------------------------------------------------------------------
    VG_REGISTER_ABSTRACT_CLASS(ShapeDesc, "Shape Settings");
    //--------------------------------------------------------------------------------------
    bool ShapeDesc::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);
        registerProperty(ShapeDesc, m_offset, "Offset");
        return true;
    }

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
}
