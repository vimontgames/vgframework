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
        return true;
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

        #pragma push_macro("new")
        #undef new
        auto shape = new JPH::BoxShape(halfSize);
        #pragma pop_macro("new")

        #pragma push_macro("new")
        #undef new
        m_shape = new JPH::RotatedTranslatedShape(JPH::Vec3(_desc.m_offset.x, _desc.m_offset.y, _desc.m_offset.z), JPH::Quat::sIdentity(), shape);
        #pragma pop_macro("new")
       
        m_transform[3].xyz = _desc.m_offset;
        m_shape->AddRef();
    }

    //--------------------------------------------------------------------------------------
    void BoxShape::Draw(const core::IWorld * _world, const core::float4x4 & _matrix)
    {
        float4x4 matrix = mul(m_transform, _matrix);
        const float3 halfSize = m_size * 0.5f;
        getDebugDraw()->AddWireframeBox(_world, -halfSize, halfSize, 0xFF0000FF, matrix);
    }
}