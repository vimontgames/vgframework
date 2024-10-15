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
    class ShapeDesc : public IShapeDesc
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(ShapeDesc, IShapeDesc);

        virtual ShapeType       GetShapeType    () const = 0;

        core::float3            getTranslation  () const;
        core::quaternion        getRotation     () const;
        core::float4x4          getMatrix       () const;

    protected:
        renderer::IDebugDraw *  getDebugDraw    () const;

    public:
        core::float3 m_rotation = core::float3(0,0,0);
        core::float3 m_offset = core::float3(0, 0, 0);
    };

    class Shape : public IShape
    {
    public:
        VG_CLASS_DECL(Shape, IShape);

        Shape(const core::string & _name, core::IObject * _parent);
        ~Shape();

        virtual void Draw(const core::IWorld * _world, const core::float4x4 & _matrix) const = 0;

        const core::float4x4 & GetTransform() const final override;
        float GetMass() const final override;

        JPH::Shape * getJoltShape() const { return m_shape; }

        void SetColor(core::u32 _color) final override { m_color = _color; }

    protected:
        renderer::IDebugDraw * getDebugDraw() const;

    protected:
        core::u32       m_color = 0xFFFFFFFF;
        core::float4x4  m_transform = core::float4x4::identity();
        JPH::Shape *    m_shape = nullptr;
    };   
}