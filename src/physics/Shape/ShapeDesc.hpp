#include "ShapeDesc.h"
#include "physics/Physics.h"
#include "renderer/IDebugDraw.h"

using namespace vg::core;

namespace vg::physics
{
    //--------------------------------------------------------------------------------------
    bool ShapeDesc::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);
        _desc.registerProperty(ShapeDesc, m_offset, "Offset");
        return true;
    }

    //--------------------------------------------------------------------------------------
    renderer::IDebugDraw * ShapeDesc::getDebugDraw() const
    {
        return Physics::get()->getDebugDraw();
    }

    //--------------------------------------------------------------------------------------
    VG_REGISTER_OBJECT_CLASS(SphereShapeDesc, "Sphere Shape Settings");
    //--------------------------------------------------------------------------------------
    bool SphereShapeDesc::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);
        _desc.registerProperty(SphereShapeDesc, m_radius, "Radius");
        return true;
    }

    //--------------------------------------------------------------------------------------
    void SphereShapeDesc::Draw(const float4x4 & _world)
    {

    }

    //--------------------------------------------------------------------------------------
    VG_REGISTER_OBJECT_CLASS(BoxShapeDesc, "Box Shape Settings");
    //--------------------------------------------------------------------------------------
    bool BoxShapeDesc::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);
        _desc.registerProperty(BoxShapeDesc, m_size, "Size");
        return true;
    }

    //--------------------------------------------------------------------------------------
    void BoxShapeDesc::Draw(const float4x4 & _world)
    {
        float4x4 transform = float4x4::identity();
        transform[3].xyz = m_offset;
        float4x4 world = mul(transform, _world);
        getDebugDraw()->AddWireframeBox(-m_size * 0.5f, m_size * 0.5f, 0xFF0000FF, world);
    }
}