#pragma once

#include "core/Object/Object.h"
#include "Physics_Consts.h"

namespace vg::physics
{
    class IShapeDesc : public core::Object
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IShapeDesc, core::Object)
        virtual ShapeType GetShapeType() const = 0;
    };

    class IShape : public core::Object
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IShape, core::Object)
        virtual ShapeType GetShapeType() const = 0;
        virtual void Draw(const core::float4x4 & _world) = 0;
        virtual const core::float4x4 & GetTransform() const = 0;
        virtual float GetMass() const = 0;
    };
}