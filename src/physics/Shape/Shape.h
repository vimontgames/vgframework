#pragma once

#include "physics/IShape.h"

namespace JPH
{
    class Shape;
}

namespace vg::renderer
{
    class IDebugDraw;
}

namespace vg::physics
{
    class SphereShapeDesc;
    class BoxShapeDesc;

    class Shape : public IShape
    {
    public:
        VG_CLASS_DECL(Shape, IShape);

        Shape(const core::string & _name, core::IObject * _parent);
        ~Shape();

        const core::float4x4 & GetTransform() const final override;

        JPH::Shape * getJoltShape() const { return m_shape; }

    protected:
        core::float4x4 m_transform = core::float4x4::identity();
        JPH::Shape * m_shape = nullptr;
    };

    class SphereShape : public Shape
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(SphereShape, Shape);

        SphereShape(const SphereShapeDesc & _desc);
    };

    class BoxShape : public Shape
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(BoxShape, Shape);

        BoxShape(const BoxShapeDesc & _desc);
    };
}