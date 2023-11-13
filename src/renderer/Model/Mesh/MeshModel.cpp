#include "renderer/Precomp.h"
#include "core/Object/AutoRegisterClass.h"
#include "MeshModel.h"
#include "gfx/Device/Device.h"
#include "gfx/Resource/Buffer.h"
#include "renderer/Geometry/Mesh/MeshGeometry.h"
#include "renderer/Importer/SceneImporterData.h"
#include "renderer/Model/Material/MaterialModel.h"
#include "shaders/system/vertex.hlsl.h"

using namespace vg::core;
using namespace vg::gfx;

namespace vg::renderer
{
    VG_AUTO_REGISTER_CLASS(MeshModel);

    //--------------------------------------------------------------------------------------
    bool MeshModel::registerClass(core::IFactory & _factory)
    {
        if (core::IClassDesc * desc = _factory.registerClassHelper(MeshModel, "Mesh Model", IClassDesc::Flags::Model))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool MeshModel::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        _desc.registerPropertyObjectRefHelper(MeshModel, m_meshGeometry, "Geometry", IProperty::Flags::NotSaved);
        _desc.registerPropertyObjectRefVectorHelper(MeshModel, m_materials, "Materials", IProperty::Flags::NotSaved);
        
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

        for (uint m = 0; m < m_materials.size(); ++m)
            VG_SAFE_RELEASE(m_materials[m]);

        m_materials.clear();
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

        vector<typename gfx::VertexStorage<F>::type> verts;
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
        if (0 == indexCount)
            return nullptr;

        const bool use32BitIndices = indexCount < 65536 ? false : true;

        const void * ibData;
        vector<u16> ib16;

        if (!use32BitIndices)
        {
            ib16.resize(indexCount);
            for (uint i = 0; i < indexCount; ++i)
                ib16[i] = (u16)_data.indices[i];

            ibData = ib16.data();
        }
        else
        {
            ibData = _data.indices.data();
        }

        BufferDesc ibDesc(Usage::Default, BindFlags::IndexBuffer, CPUAccessFlags::None, BufferFlags::None, use32BitIndices ? sizeof(u32) : sizeof(u16), indexCount);
        Buffer * ib = device->createBuffer(ibDesc, "IndexBuffer", ibData);

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

        MeshModel * meshModel = new MeshModel(_data.name, nullptr);

        MeshGeometry * meshGeometry = new MeshGeometry("MeshGeometry", meshModel);

        meshGeometry->setAABB(_data.aabb);

        meshGeometry->setIndexBuffer(ib);
        meshGeometry->setVertexBuffer(vb);

        for (uint b = 0; b < _data.batches.size(); ++b)
        {
            const auto & batch = _data.batches[b];
            meshGeometry->addBatch(batch.count, batch.offset);
        }

        VG_SAFE_RELEASE(ib);
        VG_SAFE_RELEASE(vb);

        meshModel->setGeometry(meshGeometry);

        VG_SAFE_RELEASE(meshGeometry);

        //const auto matCount = _data.materials.size();
        //for (auto m = 0; m < matCount; ++m)
        //{
        //    MaterialModel * matModel = MaterialModel::createFromImporterData(_data.materials[m]);
        //    meshModel->m_materials.push_back(matModel);
        //}

        return meshModel;
    }

    //--------------------------------------------------------------------------------------
    core::uint MeshModel::GetMaterialCount() const
    {
        return getMaterialCount();
    }

    //--------------------------------------------------------------------------------------
    IMaterialModel * MeshModel::GetMaterial(core::uint _index) const
    {
        return getMaterial(_index);
    }
}