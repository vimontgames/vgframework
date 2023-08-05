#pragma once

#include "renderer/IMeshModel.h"

namespace vg::gfx
{
    class Buffer;
    enum class VertexFormat : core::u8;
}

namespace vg::renderer
{
    class MeshGeometry;
    class MeshImporterData;
    class MaterialModel;

    class MeshModel : public IMeshModel
    {
    public:
        VG_CLASS_DECL(MeshModel, IMeshModel);

                                        MeshModel               (const core::string & _name, core::IObject * _parent);
                                        ~MeshModel              ();

        core::uint                      GetMaterialCount        () const override;
        IMaterialModel *                GetMaterial             (core::uint _index) const override;

        core::uint                      getMaterialCount        () const { return (core::uint)m_materials.size(); }
        MaterialModel *                 getMaterial             (core::uint _index) const { return _index < m_materials.size() ? m_materials[_index] : nullptr; }

        void                            setGeometry             (MeshGeometry * _meshGeometry);
        const MeshGeometry *            getGeometry             () const;

        static MeshModel *              createFromImporterData  (const MeshImporterData & _data);

    private:
        template <gfx::VertexFormat F> static gfx::Buffer * createVertexBufferFromImporterData(const MeshImporterData & _data);

    private:
        MeshGeometry *                  m_meshGeometry = nullptr;
        core::vector<MaterialModel*>    m_materials;
    };
}