#pragma once

#include "core/Object/Object.h"
#include "Physics_Consts.h"
#include "renderer/Importer/CollisionImporterData.h" // Physics should define its own format between the importer format and the runtime format to avoid bad #include

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
        virtual const core::IResource * GetResource() const = 0;
        virtual bool TryGetAABB(core::AABB & _aabb) const = 0;
    };

    class IShape : public core::Object
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IShape, core::Object)
        virtual ShapeType GetShapeType() const = 0;
        virtual core::u32 GetColor() const = 0;
        virtual void Draw(const core::IWorld * _world, const core::float4x4 & _matrix) const = 0;
        virtual const core::float4x4 & GetTransform() const = 0;
        virtual float GetMass() const = 0;
        virtual void SetColor(core::u32 _color) = 0;
        virtual void OnGeometryLoaded(const core::vector<renderer::ColliderTriangle> & _triangles) = 0;
        virtual const core::IResource * GetResource() const = 0;
    };
}