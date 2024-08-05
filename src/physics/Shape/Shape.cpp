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

        registerPropertyEx(ShapeDesc, m_rotation, "Rotate", IProperty::Flags::EulerAngle);
        setPropertyDescription(ShapeDesc, m_rotation, "Euler ZYX rotation angles");

        registerProperty(ShapeDesc, m_offset, "Offset");
        setPropertyDescription(ShapeDesc, m_offset, "Translation offset");

        return true;
    }

    //--------------------------------------------------------------------------------------
    core::float3 ShapeDesc::getTranslation() const
    {
        return m_offset;
    }

    //--------------------------------------------------------------------------------------
    core::quaternion ShapeDesc::getRotation() const
    {
        return quaternion::rotation_euler_zxy(m_rotation * (float3)(PI/180.0f));
    }

    //--------------------------------------------------------------------------------------
    core::float4x4 ShapeDesc::getMatrix() const
    {
        float4x4 m = float4x4(getRotation());
        m[3].xyz = getTranslation();
        return m;
    }

    //--------------------------------------------------------------------------------------
    Shape::Shape(const core::string & _name, core::IObject * _parent) :
        IShape(_name, _parent)
    {
    }

    //--------------------------------------------------------------------------------------
    Shape::~Shape()
    {
        VG_SAFE_RELEASE(m_shape);
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
