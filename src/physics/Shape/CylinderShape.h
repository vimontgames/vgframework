#pragma once
#include "Shape.h"

namespace vg::physics
{
    class CylinderShapeDesc final : public ShapeDesc
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(CylinderShapeDesc, ShapeDesc);
        ShapeType GetShapeType() const final override { return ShapeType::Cylinder; }
        bool TryGetAABB(core::AABB & _aabb) const final override;

        float m_radius = 0.5f;
        float m_height = 1.0f;
    };

    class CylinderShape : public Shape
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(CylinderShape, Shape);

        CylinderShape(const CylinderShapeDesc & _desc);
        ShapeType GetShapeType() const final override { return ShapeType::Cylinder; }
        void Draw(const core::IWorld * _world, const core::float4x4 & _matrix) const final override;

        float m_radius = 0.5f;
        float m_height = 1.0f;
    };
}