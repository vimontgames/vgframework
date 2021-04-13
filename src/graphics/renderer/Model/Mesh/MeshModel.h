#pragma once

#include "graphics/renderer/IMeshModel.h"

namespace vg::graphics::driver
{
    class Buffer;
    enum class VertexFormat : core::u8;
}

namespace vg::graphics::renderer
{
    class MeshGeometry;
    class MeshImporterData;

    class MeshModel : public IMeshModel
    {
        using super = IMeshModel;

    public:
        const char * getClassName() const final { return "MeshModel"; }

        static bool registerClass(core::IObjectFactory & _factory);
        static bool registerProperties(core::IObjectDescriptor & _desc);

        MeshModel(const core::string & _name, core::IObject * _parent);
        ~MeshModel();

        void setGeometry(MeshGeometry * _meshGeometry);
        const MeshGeometry * getGeometry() const;

        static MeshModel * createFromImporterData(const MeshImporterData & _data);

    private:
        template <driver::VertexFormat F> static driver::Buffer * createVertexBufferFromImporterData(const MeshImporterData & _data);

    private:
        MeshGeometry *  m_meshGeometry = nullptr;
    };
}