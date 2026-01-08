#include "BoxShape.h"

using namespace vg::core;

namespace vg::physics
{
    //--------------------------------------------------------------------------------------
    VG_REGISTER_OBJECT_CLASS(BoxShapeDesc, "Box Shape Settings");
    //--------------------------------------------------------------------------------------
    bool BoxShapeDesc::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerProperty(BoxShapeDesc, m_size, "Size");
        setPropertyRange(BoxShapeDesc, m_size, float2(0, 100));
        setPropertyDescription(BoxShapeDesc, m_size, "Box size");

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool BoxShapeDesc::TryGetAABB(AABB & _aabb) const
    {
        _aabb = AABB(-m_size.xyz * 0.5f + m_offset, m_size.xyz * 0.5f + m_offset);
        return true;
    }

    //--------------------------------------------------------------------------------------
    void BoxShapeDesc::InitializeFromAABB(const core::AABB & _aabb)
    {
        m_size = _aabb.getSize();
        m_offset = -_aabb.getCenter();
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

        if (abs(halfSize.GetX()) < shapeEps)
            halfSize.SetX(signnz(halfSize.GetX()) * shapeEps);

        if (abs(halfSize.GetY()) < shapeEps)
            halfSize.SetY(signnz(halfSize.GetY()) * shapeEps);

        if (abs(halfSize.GetZ()) < shapeEps)
            halfSize.SetZ(signnz(halfSize.GetZ()) * shapeEps);

        #pragma push_macro("new")
        #undef new
        auto shape = new JPH::BoxShape(halfSize, shapeEps);
        #pragma pop_macro("new")

        #pragma push_macro("new")
        #undef new
        m_shape = new JPH::RotatedTranslatedShape(getJoltVec3(_desc.getTranslation()), getJoltQuaternion(_desc.getRotation()), shape);
        #pragma pop_macro("new")
       
        m_transform = _desc.getMatrix();
        m_shape->AddRef();
    }

    //--------------------------------------------------------------------------------------
    void BoxShape::Draw(const core::IWorld * _world, const core::float4x4 & _matrix) const
    {
        float4x4 matrix = mul(m_transform, clearScale(_matrix));
        const float3 halfSize = m_size * 0.5f;
        getDebugDraw()->AddWireframeCube(_world, -halfSize, halfSize, m_color, matrix);
    }
}