#pragma once

#include "physics/IShape.h"

namespace vg::physics
{
    class ShapeDesc : public IShapeDesc
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(ShapeDesc, IShapeDesc);

        virtual ShapeType GetShapeType() const = 0;
        virtual void Draw(const core::float4x4 & _world) = 0;

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
        void Draw(const core::float4x4 & _world) final override;
        float m_radius = 1.0f;
    };

    class BoxShapeDesc final : public ShapeDesc
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(BoxShapeDesc, ShapeDesc);
        ShapeType GetShapeType() const final override { return ShapeType::Box; }
        void Draw(const core::float4x4 & _world) final override;
        core::float3 m_size = core::float3(1, 1, 1);
    };
}