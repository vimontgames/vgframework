#pragma once
#include "Shape.h"

namespace vg::physics
{
    class CapsuleShapeDesc final : public ShapeDesc
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(CapsuleShapeDesc, ShapeDesc);
        ShapeType GetShapeType() const final override { return ShapeType::Capsule; }
        bool TryGetAABB(core::AABB & _aabb) const final override;

        bool m_tapered = false;

        float m_bottomRadius = 0.25f;
        float m_topRadius = 0.25f;
        float m_height = 1.0f;
    };

    class CapsuleShape : public Shape
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(CapsuleShape, Shape);

        CapsuleShape(const CapsuleShapeDesc & _desc);
        ShapeType GetShapeType() const final override { return ShapeType::Capsule; }
        void Draw(const core::IWorld * _world, const core::float4x4 & _matrix) const final override;

        float m_bottomRadius = 0.25f;
        float m_topRadius = 0.25f;
        float m_height = 1.0f;
    };
}