#pragma once

#include "renderer/IMeshModel.h"
#include "shaders/system/vertex.hlsli"

namespace vg::gfx
{
    class Buffer;
    class BLAS;
    using BLASVariantKey = core::u64;
    using BLASMap = core::unordered_map<BLASVariantKey, BLAS *>;
}

namespace vg::renderer
{
    class MeshImporterData;
    class MeshGeometry;
    class Skeleton;

    class MeshModel : public IMeshModel
    {
    public:
        VG_CLASS_DECL(MeshModel, IMeshModel);

                                                MeshModel               (const core::string & _name, core::IObject * _parent);
                                                ~MeshModel              ();

        core::uint                              GetBatchCount           () const final override;
        core::vector<core::string>              GetBatchNames           () const final override;

        void                                    setGeometry             (MeshGeometry * _geometry);
        const MeshGeometry *                    getGeometry             () const;

        void                                    setSkeleton             (Skeleton * _skeleton);
        const Skeleton *                        getSkeleton             () const;

        void                                    clearBLASes             ();
        gfx::BLASMap &                          getBLASMap              () { return m_BLASMap; }

        void                                    setColliderTriangles    (const core::vector<ColliderTriangle> & _triangles);
        const core::vector<ColliderTriangle> &  GetColliderTriangles    () const final override { return m_colliderTriangles; }

        static MeshModel *                      createFromImporterData  (const MeshImporterData & _data);

    private:
        template <VertexFormat F> static gfx::Buffer * createVertexBufferFromImporterData(const MeshImporterData & _data);

    private:
        MeshGeometry *                          m_geometry  = nullptr;
        Skeleton *                              m_skeleton  = nullptr;
        gfx::BLASMap                            m_BLASMap;
        core::vector<ColliderTriangle>          m_colliderTriangles;
    };
}