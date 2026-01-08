#include "CylinderShape.h"

namespace vg::physics
{
    //--------------------------------------------------------------------------------------
    VG_REGISTER_OBJECT_CLASS(CylinderShapeDesc, "Cylinder Shape Settings");
    //--------------------------------------------------------------------------------------
    bool CylinderShapeDesc::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerProperty(CylinderShapeDesc, m_radius, "Radius");
        setPropertyRange(CylinderShapeDesc, m_radius, float2(0.01f, 100.0f));
        setPropertyDescription(CylinderShapeDesc, m_radius, "Cylinder radius");

        registerProperty(CylinderShapeDesc, m_height, "Height");
        setPropertyRange(CylinderShapeDesc, m_height, float2(0.01f, 100.0f));
        setPropertyDescription(CylinderShapeDesc, m_height, "Cylinder height");

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool CylinderShapeDesc::TryGetAABB(AABB & _aabb) const
    {
        _aabb = AABB(-float3(m_height * 0.5f, m_height * 0.5f, m_height * 0.5f) + m_offset, float3(m_height * 0.5f, m_height * 0.5f, m_height * 0.5f) + m_offset);
        return true;
    }

    //--------------------------------------------------------------------------------------
    VG_REGISTER_OBJECT_CLASS(CylinderShape, "Cylinder Shape");
    //--------------------------------------------------------------------------------------
    bool CylinderShape::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);
        return true;
    }

    //--------------------------------------------------------------------------------------
    CylinderShape::CylinderShape(const CylinderShapeDesc & _desc) :
        Shape("Cylinder", nullptr),
        m_radius(_desc.m_radius),
        m_height(_desc.m_height)
    {        
        JPH::ConvexShape * shape = nullptr;
        JPH::Shape::ShapeResult result;
        
        #pragma push_macro("new")
        #undef new

        JPH::CylinderShapeSettings settings(max(m_height * 0.5f, shapeEps), max(m_radius, shapeEps));
        shape = new JPH::CylinderShape(settings, result);
        if (result.HasError())
            VG_WARNING("[Physics] %s", result.GetError().c_str());
        #pragma pop_macro("new")
        
        #pragma push_macro("new")
        #undef new
        m_shape = new JPH::RotatedTranslatedShape(getJoltVec3(_desc.getTranslation()), getJoltQuaternion(_desc.getRotation()) * JPH::Quat::sRotation(JPH::Vec3(1, 0, 0), PI / 2.0f), shape);
        #pragma pop_macro("new")
        
        m_transform = _desc.getMatrix();
        m_shape->AddRef();
    }

    //--------------------------------------------------------------------------------------
    void CylinderShape::Draw(const core::IWorld * _world, const core::float4x4 & _matrix) const
    {
        float4x4 matrix = mul(m_transform, clearScale(_matrix));
        getDebugDraw()->AddWireframeCylinder(_world, m_radius, m_height, m_color, matrix);
    }
}
