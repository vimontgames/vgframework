#include "graphics/renderer/Precomp.h"
#include "core/Object/AutoRegisterClass.h"
#include "MeshModel.h"
#include "graphics/driver/Device/Device.h"
#include "graphics/driver/Resource/Buffer.h"
#include "graphics/renderer/Geometry/Mesh/MeshGeometry.h"
#include "graphics/renderer/Importer/SceneImporterData.h"
#include "shaders/system/vertex.hlsl.h"

using namespace vg::core;
using namespace vg::graphics::driver;

namespace vg::graphics::renderer
{
    VG_AUTO_REGISTER_CLASS(MeshModel);

    //--------------------------------------------------------------------------------------
    bool MeshModel::registerClass(core::IObjectFactory & _factory)
    {
        if (core::IObjectDescriptor * desc = _factory.registerClassHelper(MeshModel, "Mesh Model"))
            registerProperties(*desc);

        return true;
    }


    //--------------------------------------------------------------------------------------
    bool MeshModel::registerProperties(core::IObjectDescriptor & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    MeshModel::MeshModel(const core::string & _name, core::IObject * _parent) :
        IMeshModel(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    MeshModel::~MeshModel()
    {
        VG_SAFE_RELEASE(m_meshGeometry);
    }

    //--------------------------------------------------------------------------------------
    void MeshModel::setGeometry(MeshGeometry * _meshGeometry)
    {
        if (_meshGeometry != m_meshGeometry)
        {
            VG_SAFE_INCREASE_REFCOUNT(_meshGeometry);
            VG_SAFE_RELEASE(m_meshGeometry);
            m_meshGeometry = _meshGeometry;
        }
    }

    //--------------------------------------------------------------------------------------
    const MeshGeometry * MeshModel::getGeometry() const
    {
        return m_meshGeometry;
    }

    //--------------------------------------------------------------------------------------
    template <VertexFormat F> Buffer * MeshModel::createVertexBufferFromImporterData(const MeshImporterData & _data)
    {
        auto * device = Device::get();

        const uint vertexCount = (uint)_data.vertices.size();

        vector<driver::VertexStorage<F>::type> verts;
                                               verts.resize(vertexCount);

        for (uint i = 0; i < vertexCount; ++i)
            verts[i].set(_data.vertices[i]);

        BufferDesc vbDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::None, BufferFlags::None, sizeof(VertexStorage<F>::type), vertexCount);
        Buffer * vb = device->createBuffer(vbDesc, "VertexBuffer", verts.data());

        return vb;
    }

    //--------------------------------------------------------------------------------------
    MeshModel * MeshModel::createFromImporterData(const MeshImporterData & _data)
    {
        auto * device = Device::get();

        const uint indexCount = (uint)_data.indices.size();
        vector<u16> ib16;
                    ib16.resize(indexCount);
        for (uint i = 0; i < indexCount; ++i)
            ib16[i] = (u16)_data.indices[i];

        BufferDesc ibDesc(Usage::Default, BindFlags::IndexBuffer, CPUAccessFlags::None, BufferFlags::None, sizeof(u16), indexCount);
        Buffer * ib = device->createBuffer(ibDesc, "IndexBuffer", ib16.data());

        Buffer * vb = nullptr;

        // TODO: select appropriate vertex format
        VertexFormat vtxFmt = VertexFormat::Simple;

        switch (vtxFmt)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(vtxFmt);

            case VertexFormat::Simple:
                vb = createVertexBufferFromImporterData<VertexFormat::Simple>(_data);
                break;
        }

        MeshModel * meshModel = new MeshModel("MeshModel", nullptr);

        MeshGeometry * meshGeometry = new MeshGeometry("MeshGeometry", meshModel);
        meshGeometry->setIndexBuffer(ib);
        meshGeometry->setVertexBuffer(vb);
        meshGeometry->addBatch(indexCount, 0);

        VG_SAFE_RELEASE(ib);
        VG_SAFE_RELEASE(vb);

        meshModel->setGeometry(meshGeometry);

        VG_SAFE_RELEASE(meshGeometry);

        return meshModel;
    }
}