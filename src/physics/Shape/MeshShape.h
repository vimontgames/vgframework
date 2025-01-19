#pragma once
#include "Shape.h"

namespace vg::physics
{
    class MeshShapeDesc final : public ShapeDesc
    {
    public:
        VG_CLASS_DECL(MeshShapeDesc, ShapeDesc);

        MeshShapeDesc(const vg::core::string & _name = "", vg::core::IObject * _parent = nullptr);
        ~MeshShapeDesc();

        ShapeType GetShapeType() const final override { return ShapeType::Mesh; }
        bool TryGetAABB(core::AABB & _aabb) const final override;
        const core::IResource * GetResource() const final override { return m_meshResource; }

    private:
        core::IResource * m_meshResource = nullptr;
    };

    class MeshShape : public Shape
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(MeshShape, Shape);

        MeshShape(const MeshShapeDesc & _desc);
        ~MeshShape();

        ShapeType GetShapeType() const final override { return ShapeType::Mesh; }
        void Draw(const core::IWorld * _world, const core::float4x4 & _matrix) const final override;
        void OnGeometryLoaded(const core::vector<renderer::ColliderTriangle> & _triangles) final override;
        const core::IResource * GetResource() const final override { return m_meshResource; }
        float GetMass() const final override;

    private:
        core::IResource *   m_meshResource = nullptr;
        core::quaternion    m_rotation = core::quaternion(0, 0, 0, 1);
        core::float3        m_translation = core::float3(0, 0, 0);
        float               m_mass = 0.0f;
    };
}