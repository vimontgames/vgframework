#pragma once

#include "renderer/IMeshModel.h"
#include "shaders/system/vertex.hlsli"

namespace vg::gfx
{
    class Buffer;
    class BLAS;

    using BLASCollectionKey = core::u64;
    class BLASCollection;
    using BLASCollectionMap = core::unordered_map<BLASCollectionKey, BLASCollection *>;
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

        void                            setGeometry             (MeshGeometry * _geometry);
        const MeshGeometry *            getGeometry             () const;

        void                            setSkeleton             (Skeleton * _skeleton);
        const Skeleton *                getSkeleton             () const;

        void                            clearBLASes             ();
        gfx::BLASCollectionMap &        getBLASCollectionMap    () { return m_BLASCollectionMap; }

        static MeshModel *              createFromImporterData  (const MeshImporterData & _data);

    private:
        template <VertexFormat F> static gfx::Buffer * createVertexBufferFromImporterData(const MeshImporterData & _data);

    private:
        MeshGeometry *                  m_geometry  = nullptr;
        Skeleton *                      m_skeleton  = nullptr;
        gfx::BLASCollectionMap          m_BLASCollectionMap;
    };
}