#pragma once

#include "renderer/IMeshModel.h"
#include "shaders/system/vertex.hlsli"

namespace vg::gfx
{
    class Buffer;
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

        static MeshModel *              createFromImporterData  (const MeshImporterData & _data);

    private:
        template <VertexFormat F> static gfx::Buffer * createVertexBufferFromImporterData(const MeshImporterData & _data);

    private:
        MeshGeometry *                  m_geometry = nullptr;
        Skeleton *                      m_skeleton = nullptr;
    };
}