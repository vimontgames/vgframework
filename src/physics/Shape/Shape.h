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

        virtual void Draw(const core::float4x4 & _world) = 0;

        const core::float4x4 & GetTransform() const final override;
        float GetMass() const final override;

        JPH::Shape * getJoltShape() const { return m_shape; }

    protected:
        renderer::IDebugDraw * getDebugDraw() const;

    protected:
        core::float4x4 m_transform = core::float4x4::identity();
        JPH::Shape * m_shape = nullptr;
    };

    class SphereShape : public Shape
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(SphereShape, Shape);

        SphereShape(const SphereShapeDesc & _desc);

        void Draw(const core::float4x4 & _world) final override;

        float m_radius = 0.0f;
    };

    class BoxShape : public Shape
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(BoxShape, Shape);

        BoxShape(const BoxShapeDesc & _desc);

        void Draw(const core::float4x4 & _world) final override;

        core::float3 m_size = core::float3(0, 0, 0);
    };
}