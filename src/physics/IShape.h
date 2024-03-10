#pragma once

#include "core/Object/Object.h"
#include "Physics_Consts.h"

namespace vg::core
{
    class IWorld;
    class AABB;
}

namespace vg::physics
{
    class IShapeDesc : public core::Object
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IShapeDesc, core::Object)
        virtual ShapeType GetShapeType() const = 0;
        virtual bool TryGetAABB(core::AABB & _aabb) const = 0;
    };

    class IShape : public core::Object
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IShape, core::Object)
        virtual ShapeType GetShapeType() const = 0;
        virtual void Draw(const core::IWorld * _world, const core::float4x4 & _matrix) = 0;
        virtual const core::float4x4 & GetTransform() const = 0;
        virtual float GetMass() const = 0;
    };
}