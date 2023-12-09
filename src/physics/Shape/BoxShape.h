#pragma once
#include "Shape.h"

namespace vg::physics
{
    class BoxShapeDesc final : public ShapeDesc
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(BoxShapeDesc, ShapeDesc);
        ShapeType GetShapeType() const final override { return ShapeType::Box; }
        core::float3 m_size = core::float3(1, 1, 1);
    };

    class BoxShape : public Shape
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(BoxShape, Shape);

        BoxShape(const BoxShapeDesc & _desc);
        ShapeType GetShapeType() const final override { return ShapeType::Box; }
        void Draw(const core::float4x4 & _world) final override;
        core::float3 m_size = core::float3(0, 0, 0);
    };
}