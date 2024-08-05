#pragma once
#include "Shape.h"

namespace vg::physics
{
    class SphereShapeDesc final : public ShapeDesc
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(SphereShapeDesc, ShapeDesc);
        ShapeType GetShapeType() const final override { return ShapeType::Sphere; }
        bool TryGetAABB(core::AABB & _aabb) const final override;
        float m_radius = 0.5f;
    };

    class SphereShape : public Shape
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(SphereShape, Shape);

        SphereShape(const SphereShapeDesc & _desc);
        ShapeType GetShapeType() const final override { return ShapeType::Sphere; }
        void Draw(const core::IWorld * _world, const core::float4x4 & _matrix) final override;
        float m_radius = 0.5f;
    };
}