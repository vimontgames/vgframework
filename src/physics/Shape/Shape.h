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
        VG_CLASS_DECL_ABSTRACT_WITH_PROPERTIES(ShapeDesc, IShapeDesc);

        ShapeType               GetShapeType        () const override = 0;
        const core::IResource * GetResource         () const override { return nullptr; }
        void                    InitializeFromAABB  (const core::AABB & _aabb) override { VG_ASSERT_NOT_IMPLEMENTED(); };

        core::float3            getTranslation      () const;
        core::quaternion        getRotation         () const;
        core::float4x4          getMatrix           () const;

    protected:
        renderer::IDebugDraw *  getDebugDraw        () const;

    public:
        core::float3 m_rotation = core::float3(0,0,0);
        core::float3 m_offset = core::float3(0, 0, 0);
    };

    class Shape : public IShape
    {
    public:
        VG_CLASS_DECL_ABSTRACT_WITH_CTOR_AND_PROPERTIES(Shape, IShape);

        Shape(const core::string & _name, core::IObject * _parent);
        ~Shape();

        void Draw(const core::IWorld * _world, const core::float4x4 & _matrix) const override = 0;
        void OnGeometryLoaded(const core::vector<renderer::ColliderTriangle> & _triangles) override {}
        const core::IResource * GetResource() const override { return nullptr; }

        const core::float4x4 & GetTransform() const final override;
        float GetMass() const override;

        JPH::Shape * getJoltShape() const { return m_shape; }

        void SetColor(core::u32 _color) final override { m_color = _color; }
        core::u32 GetColor() const final override { return m_color; }

    protected:
        renderer::IDebugDraw * getDebugDraw() const;

    protected:
        core::u32       m_color = 0xFFFFFFFF;
        core::float4x4  m_transform = core::float4x4::identity();
        JPH::Shape *    m_shape = nullptr;
    };   
}