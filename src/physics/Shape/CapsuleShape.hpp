#include "CapsuleShape.h"

namespace vg::physics
{
    //--------------------------------------------------------------------------------------
    VG_REGISTER_OBJECT_CLASS(CapsuleShapeDesc, "Capsule Shape Settings");
    //--------------------------------------------------------------------------------------
    bool CapsuleShapeDesc::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerOptionalProperty(CapsuleShapeDesc, m_tapered, m_topRadius, "Top radius");
        setPropertyRange(CapsuleShapeDesc, m_topRadius, float2(0.01f, 100.0f));
        setPropertyDescription(CapsuleShapeDesc, m_topRadius, "Capsule radius at bottom");

        registerProperty(CapsuleShapeDesc, m_bottomRadius, "Bottom Radius");
        setPropertyRange(CapsuleShapeDesc, m_bottomRadius, float2(0.01f, 100.0f));
        setPropertyDescription(CapsuleShapeDesc, m_bottomRadius, "Capsule radius at bottom");

        registerProperty(CapsuleShapeDesc, m_height, "Height");
        setPropertyRange(CapsuleShapeDesc, m_height, float2(0.01f, 100.0f));
        setPropertyDescription(CapsuleShapeDesc, m_height, "Capsule height");

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
        m_topRadius(_desc.m_tapered ? _desc.m_topRadius : _desc.m_bottomRadius),
        m_bottomRadius(_desc.m_bottomRadius),
        m_height(_desc.m_height)
    {
        float height = m_height - (m_topRadius + m_bottomRadius);

        JPH::ConvexShape * shape = nullptr;
        JPH::Shape::ShapeResult result;

        #pragma push_macro("new")
        #undef new
        if (_desc.m_tapered && m_topRadius != m_bottomRadius)
        {
            if (max(m_topRadius, m_bottomRadius) >= height + min(m_topRadius, m_bottomRadius))
            {
                // One sphere embedded in other sphere, using sphere shape instead
                shape = new JPH::SphereShape(max(shapeEps, max(m_topRadius, m_bottomRadius)));
            }
            else
            {

                JPH::TaperedCapsuleShapeSettings settings(max(shapeEps, height) * 0.5f, m_topRadius, m_bottomRadius);
                shape = new JPH::TaperedCapsuleShape(settings, result);
                if (result.HasError())
                    VG_WARNING("[Physics] %s", result.GetError().c_str());
            }
        }
        else
        {
            if (height > 0.0f)
                shape = new JPH::CapsuleShape(max(shapeEps, height) * 0.5f, max(shapeEps, m_bottomRadius));
            else
                shape = new JPH::SphereShape(max(shapeEps, m_bottomRadius));
        }
        #pragma pop_macro("new")

        #pragma push_macro("new")
        #undef new
        m_shape = new JPH::RotatedTranslatedShape(getJoltVec3(_desc.getTranslation()), getJoltQuaternion(_desc.getRotation()) * JPH::Quat::sRotation(JPH::Vec3(1, 0, 0), PI / 2.0f), shape);
        #pragma pop_macro("new")

        m_transform = _desc.getMatrix();
        m_shape->AddRef();
    }

    //--------------------------------------------------------------------------------------
    void CapsuleShape::Draw(const core::IWorld * _world, const core::float4x4 & _matrix) const
    {
        float4x4 matrix = mul(m_transform, getMatrixWithoutScale(_matrix));

        if (m_bottomRadius != m_topRadius)
            getDebugDraw()->AddTaperedCapsule(_world, m_topRadius, m_bottomRadius, m_height, m_color, matrix);
        else
            getDebugDraw()->AddCapsule(_world, m_bottomRadius, m_height, m_color, matrix);
    }
}
