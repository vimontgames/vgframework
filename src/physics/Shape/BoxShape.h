#pragma once
#include "Shape.h"

namespace vg::physics
{
    class BoxShapeDesc final : public ShapeDesc
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(BoxShapeDesc, ShapeDesc);
        ShapeType   GetShapeType        () const final override { return ShapeType::Box; }
        bool        TryGetAABB          (core::AABB & _aabb) const final override;
        void        InitializeFromAABB  (const core::AABB & _aabb) final override;

        core::float3 m_size = core::float3(1, 1, 1);
    };

    class BoxShape : public Shape
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(BoxShape, Shape);

        BoxShape(const BoxShapeDesc & _desc);
        ShapeType GetShapeType() const final override { return ShapeType::Box; }
        void Draw(const core::IWorld * _world, const core::float4x4 & _matrix) const final override;
        core::float3 m_size = core::float3(1, 1, 1);
    };
}