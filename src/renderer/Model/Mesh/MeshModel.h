#pragma once

#include "core/IResource.h"
#include "renderer/IMeshModel.h"
#include "Shaders/system/vertex.hlsli"

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

        static core::LoadStatus                 createFromImporterData  (const MeshImporterData & _data, IMeshModel *& _meshModel);

    private:
        template <VertexFormat F> static core::LoadStatus createVertexBufferFromImporterData(const MeshImporterData & _data, gfx::Buffer *& _buffer);

    private:
        MeshGeometry *                          m_geometry  = nullptr;
        Skeleton *                              m_skeleton  = nullptr;
        gfx::BLASMap                            m_BLASMap;
        core::vector<ColliderTriangle>          m_colliderTriangles;
    };
}