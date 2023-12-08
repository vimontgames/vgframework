#pragma once

#include "physics/IShape.h"

namespace vg::physics
{
    class ShapeDesc : public IShapeDesc
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(ShapeDesc, IShapeDesc);

        virtual ShapeType GetShapeType() const = 0;

    protected:
        renderer::IDebugDraw * getDebugDraw() const;

    public:
        core::float3 m_offset = core::float3(0, 0, 0);
    };

    class SphereShapeDesc final : public ShapeDesc
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(SphereShapeDesc, ShapeDesc);
        ShapeType GetShapeType() const final override { return ShapeType::Sphere; }

        float m_radius = 1.0f;
    };

    class BoxShapeDesc final : public ShapeDesc
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(BoxShapeDesc, ShapeDesc);
        ShapeType GetShapeType() const final override { return ShapeType::Box; }

        core::float3 m_size = core::float3(1, 1, 1);
    };

    class CapsuleShapeDesc final : public ShapeDesc
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(CapsuleShapeDesc, ShapeDesc);
        ShapeType GetShapeType() const final override { return ShapeType::Capsule; }

        float m_radius = 1.0f;
        float m_height = 1.0f;
    };

    
}