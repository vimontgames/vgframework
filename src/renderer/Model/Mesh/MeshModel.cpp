#include "renderer/Precomp.h"
#include "core/Object/AutoRegisterClass.h"
#include "MeshModel.h"
#include "gfx/Device/Device.h"
#include "gfx/Resource/Buffer.h"
#include "gfx/Raytracing/BLAS.h"
#include "renderer/Geometry/Mesh/MeshGeometry.h"
#include "renderer/Geometry/Vertex/VertexFormat.h"
#include "renderer/Importer/SceneImporterData.h"
#include "renderer/Animation/Skeleton.h"
#include "renderer/RayTracing/RayTracingManager.h"

using namespace vg::core;
using namespace vg::gfx;

namespace vg::renderer
{
    VG_REGISTER_OBJECT_CLASS_EX(MeshModel, "Mesh Model", ClassDescFlags::Model);

    //--------------------------------------------------------------------------------------
    bool MeshModel::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyGroupBegin(MeshModel, "Geometry");
        {
            registerPropertyObjectPtrEx(MeshModel, m_geometry, "Geometry", PropertyFlags::Flatten | PropertyFlags::Transient);
        }
        registerPropertyGroupEnd(MeshModel);

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
        VG_SAFE_RELEASE(m_geometry); 
        VG_SAFE_RELEASE(m_skeleton);

        clearBLASes();

        auto * rtManager = RayTracingManager::get(false);
        if (rtManager)
            rtManager->removeMeshModel(this);
    }

    //--------------------------------------------------------------------------------------
    uint MeshModel::GetBatchCount() const
    {
        if (auto * geo = getGeometry())
            return (uint)geo->batches().size();
        else
            return 0;
    }

    //--------------------------------------------------------------------------------------
    core::vector<core::string> MeshModel::GetBatchNames() const
    {
        core::vector<core::string> names;
        if (const auto * geo = getGeometry())
        {
            const auto batches = geo->batches();
            names.resize(batches.size());
            for (uint i = 0; i < batches.size(); ++i)
                names[i] = batches[i].GetName();
        }
        return names;
    }

    //--------------------------------------------------------------------------------------
    // clearing BLAS collections when deleting model isn't necessary because only instances
    // using it will increase RefCount and this way we don't have to deal with dandling pointers
    // we are sure that when all instances are deleted then all BLAS collections are deleted too
    //--------------------------------------------------------------------------------------
    void MeshModel::clearBLASes()
    {
        for (auto & BLASes : m_BLASMap)
            VG_SAFE_RELEASE(BLASes.second);
        m_BLASMap.clear();
    }

    //--------------------------------------------------------------------------------------
    void MeshModel::setGeometry(MeshGeometry * _geometry)
    {
        if (_geometry != m_geometry)
        {
            VG_SAFE_INCREASE_REFCOUNT(_geometry);
            VG_SAFE_RELEASE(m_geometry);
            m_geometry = _geometry;
        }
    }

    //--------------------------------------------------------------------------------------
    const MeshGeometry * MeshModel::getGeometry() const
    {
        return m_geometry;
    }

    //--------------------------------------------------------------------------------------
    void MeshModel::setSkeleton(Skeleton * _skeleton)
    {
        if (_skeleton != m_skeleton)
        {
            VG_SAFE_INCREASE_REFCOUNT(_skeleton);
            VG_SAFE_RELEASE(m_skeleton);
            m_skeleton = _skeleton;
        }
    }

    //--------------------------------------------------------------------------------------
    const Skeleton * MeshModel::getSkeleton() const
    {
        return m_skeleton;
    }

    //--------------------------------------------------------------------------------------
    template <VertexFormat F> LoadStatus MeshModel::createVertexBufferFromImporterData(const MeshImporterData & _data, Buffer *& _buffer)
    {
        auto * device = Device::get();

        const uint vertexCount = (uint)_data.vertices.size();

        vector<typename VertexStorage<F>::type> verts;
                                                verts.resize(vertexCount);

        for (uint i = 0; i < vertexCount; ++i)
            verts[i].set(_data.vertices[i]);

        BufferDesc vbDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::None, BufferFlags::None, sizeof(VertexStorage<F>::type), vertexCount);

        return device->createResourceBuffer(vbDesc, "VertexBuffer", verts.data(), _buffer);
    }

    //--------------------------------------------------------------------------------------
    LoadStatus MeshModel::createFromImporterData(const MeshImporterData & _data, IMeshModel *& _meshModel)
    {
        auto * device = Device::get();

        const uint indexCount = (uint)_data.indices.size();
        if (0 == indexCount)
            return LoadStatus::Success;

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

        BufferDesc ibDesc(Usage::Default, BindFlags::IndexBuffer | BindFlags::ShaderResource, CPUAccessFlags::None, BufferFlags::None, use32BitIndices ? sizeof(u32) : sizeof(u16), indexCount);
        Buffer * ib = nullptr;
        LoadStatus status = device->createResourceBuffer(ibDesc, "IndexBuffer", ibData, ib);
        if (status != LoadStatus::Success)
        {
            _meshModel = nullptr;
            return status;
        }

        Buffer * vb = nullptr;

        // Select vertex format (TODO: select vertex format at cooking time?)
        VertexFormat vtxFmt;

        if (_data.bonesIndices.size() > 0)
        {
            switch (_data.maxBonesCountPerVertex)
            {
                default:
                VG_ASSERT(_data.maxBonesCountPerVertex == 4);
                break;
        
                case 4:
                vtxFmt = VertexFormat::Skinning_4Bones;
                break;
            }
        }
        else
            vtxFmt = VertexFormat::Default;

        switch (vtxFmt)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(vtxFmt);

            case VertexFormat::Default:
                status = createVertexBufferFromImporterData<VertexFormat::Default>(_data, vb);
                if (status != LoadStatus::Success)
                    return status;
                break;

            case VertexFormat::Skinning_4Bones:
                status = createVertexBufferFromImporterData<VertexFormat::Skinning_4Bones>(_data, vb);
                if (status != LoadStatus::Success)
                    return status;
                break;
        }

        MeshModel * meshModel = new MeshModel(_data.name, nullptr);
        meshModel->RegisterUID();

        MeshGeometry * meshGeometry = new MeshGeometry(_data.name, meshModel);

        meshGeometry->setAABB(_data.aabb);
        meshGeometry->setVertexFormat(vtxFmt);
        meshGeometry->setIndexBuffer(ib);
        meshGeometry->setVertexBuffer(vb);

        for (uint b = 0; b < _data.batches.size(); ++b)
        {
            const auto & batch = _data.batches[b];
            meshGeometry->addBatch(batch.GetName(), batch.count, batch.offset);
        }

        VG_SAFE_RELEASE(ib);
        VG_SAFE_RELEASE(vb);

        meshModel->setGeometry(meshGeometry);
        VG_SAFE_RELEASE(meshGeometry);

        Skeleton * meshSkeleton = nullptr;
        if (_data.bonesIndices.size() > 0)
        {
            meshSkeleton = new Skeleton("Skeleton", meshModel);
            meshSkeleton->setNodes(_data.nodes);
            meshSkeleton->setBoneIndices(_data.bonesIndices);
            meshSkeleton->setBoneMatrices(_data.bonesMatrices);
        }
        meshModel->setSkeleton(meshSkeleton);
        VG_SAFE_RELEASE(meshSkeleton);

        RayTracingManager::get()->addMeshModel(meshModel);

        meshModel->setColliderTriangles(_data.colliderTriangles);

        _meshModel = meshModel;
        return LoadStatus::Success;
    }

    //--------------------------------------------------------------------------------------
    void MeshModel::setColliderTriangles(const core::vector<ColliderTriangle> & _triangles)
    {
        m_colliderTriangles = _triangles;
    }
}