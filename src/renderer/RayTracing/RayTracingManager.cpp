#include "renderer/Precomp.h"
#include "RayTracingManager.h"
#include "core/Timer/Timer.h"
#include "gfx/CommandList/CommandList.h"
#include "gfx/Resource/Buffer.h"
#include "renderer/Model/Mesh/MeshModel.h"
#include "renderer/Geometry/Mesh/MeshGeometry.h"
#include "gfx/Raytracing/TLAS.h"
#include "gfx/Raytracing/BLAS.h"
#include "renderer/Renderer.h"
#include "renderer/View/View.h"
#include "renderer/View/Shadow/ShadowView.h"
#include "renderer/Instance/Mesh/MeshInstance.h"
#include "renderer/Model/Material/MaterialModel.h"

using namespace vg::core;

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    bool RayTracingManager::enableRayTracing(bool _enable)
    {
        if (_enable != m_rayTracingEnabled)
        {
            if (_enable)
                onEnableRayTracing();
            else
                onDisableRayTracing();

            m_rayTracingEnabled = _enable;
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool RayTracingManager::isRayTracingEnabled() const
    {
        return m_rayTracingEnabled;
    }

    //--------------------------------------------------------------------------------------
    // Can't create BLAS right after enabling RayTracing because the actual BLASes to use 
    // will depend on the materials' surface types, so instead we create a BLASCollection 
    // hash that will be populated on-demand.
    //--------------------------------------------------------------------------------------
    void RayTracingManager::onEnableRayTracing()
    {
        VG_INFO("[Renderer] RayTracing is enabled");
        
        for (uint i = 0; i < m_meshInstances.size(); ++i)
        {
            MeshInstance * meshInstance = m_meshInstances[i];
            meshInstance->updateInstanceBLAS();
        }
    }

    //--------------------------------------------------------------------------------------
    void RayTracingManager::onDisableRayTracing()
    {
        VG_INFO("[Renderer] RayTracing is disabled");

        for (uint i = 0; i < m_meshInstances.size(); ++i)
        {
            MeshInstance * meshInstance = m_meshInstances[i];
            meshInstance->setInstanceBLAS(nullptr);
        }

        for (uint i = 0; i < m_meshModels.size(); ++i)
        {
            MeshModel * meshModel = m_meshModels[i];
            meshModel->clearBLASes();
        }

        auto * renderer = Renderer::get();
        for (uint i = 0; i < enumCount<gfx::ViewTarget>(); ++i)
        {
            auto target = (gfx::ViewTarget)i;
            auto views = renderer->GetViews(target);
            for (uint j = 0; j < views.size(); ++j)
            {
                auto view = ((View *)views[j]);
                view->setTLAS(nullptr);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void RayTracingManager::addMeshModel(MeshModel * _meshModel)
    {
        VG_ASSERT(!m_meshModels.exists(_meshModel));
        m_meshModels.push_back(_meshModel);
    
        //if (isRayTracingEnabled())
        //    updateMeshModel(_meshModel);
    }
    
    //--------------------------------------------------------------------------------------
    //void RayTracingManager::updateMeshModel(MeshModel * _meshModel)
    //{
    //    VG_ASSERT(!m_meshModelUpdateQueue.exists(_meshModel));
    //    m_meshModelUpdateQueue.push_back(_meshModel);
    //}
    
    //--------------------------------------------------------------------------------------
    void RayTracingManager::removeMeshModel(MeshModel * _meshModel)
    {
        VG_ASSERT(m_meshModels.exists(_meshModel));
        m_meshModels.remove(_meshModel);
    }

    //--------------------------------------------------------------------------------------
    void RayTracingManager::addMeshInstance(MeshInstance * _meshInstance)
    {
        VG_ASSERT(!m_meshInstances.exists(_meshInstance));
        m_meshInstances.push_back(_meshInstance);
    }

    //--------------------------------------------------------------------------------------
    void RayTracingManager::removeMeshInstance(MeshInstance * _meshInstance)
    {
        VG_ASSERT(m_meshInstances.exists(_meshInstance));
        m_meshInstances.remove(_meshInstance);
    }

    //--------------------------------------------------------------------------------------
    void RayTracingManager::updateMeshInstance(MeshInstance * _meshInstance)
    {
        // TODO : Use 'DirtyBLAS' + atomic to avoid search?
        if(!m_meshInstanceUpdateQueue.exists(_meshInstance))
            m_meshInstanceUpdateQueue.push_back(_meshInstance);
    }

    //--------------------------------------------------------------------------------------
    void RayTracingManager::update(gfx::CommandList * _cmdList, gfx::Buffer * _skinningBuffer)
    {
        if (isRayTracingEnabled())
        {
            // Non-skins require update when model or material surface types change, but BLAS can be shared
            for (uint i = 0; i < m_meshInstanceUpdateQueue.size(); ++i)
            {
                MeshInstance * instance = m_meshInstanceUpdateQueue[i];
                VG_ASSERT(!instance->IsSkinned());
                auto * blas = instance->getInstanceBLAS();
                MeshModel * meshModel = (MeshModel*)instance->getMeshModel(Lod::Lod0);

                // Create BLAS collection if it does not exist yet or it's key changed
                gfx::BLASVariantKey key = instance->computeBLASVariantKey();

                if (nullptr == blas || blas->getKey() != key)
                {
                    // Try to find a corresponding BLAS in map
                    gfx::BLASMap & BLASMap = meshModel->getBLASMap();

                    auto it = BLASMap.find(key);
                    if (BLASMap.end() != it)
                    {
                        blas = it->second;
                        instance->setInstanceBLAS(blas);    // This will increase RefCount
                        VG_INFO("[Renderer] Use existing BLAS 0x%016X for instance \"%s\" with key 0x%016X (RefCount=%u)", it->second, instance->getName().c_str(), key, blas->getRefCount());
                    }
                    else
                    {
                        // Create new BLAS collection and add it to hash for reuse
                        blas = new gfx::BLAS(gfx::BLASUpdateType::Static, key);
                        instance->setInstanceBLAS(blas);    // This will increase RefCount
                        blas->Release();
                        blas->clear();

                        const MeshGeometry * meshGeo = meshModel->getGeometry();
                        gfx::Buffer * ib = meshGeo->getIndexBuffer();
                        const uint ibOffset = meshGeo->getIndexBufferOffset();

                        const gfx::Buffer * modelVB = meshGeo->getVertexBuffer();

                        VertexFormat vtxFmt = meshGeo->getVertexFormat();
                        const uint vertexStride = getVertexFormatStride(vtxFmt);

                        const auto batches = meshGeo->batches();
                        const auto & materials = instance->getMaterials();

                        for (uint m = 0; m < batches.size(); ++m)
                        {
                            gfx::SurfaceType surfaceType = gfx::SurfaceType::Opaque;
                            const MaterialModel * mat = m < materials.size() ? materials[m] : nullptr;
                            if (mat)
                                surfaceType = mat->getSurfaceType();

                            const Batch & batch = batches[m];
                            VG_ASSERT(blas);
                            blas->addIndexedGeometry(ib, ibOffset + batch.offset, batch.count, modelVB, 0, modelVB->getBufDesc().getElementCount(), vertexStride, surfaceType);
                        }

                        blas->update(_cmdList);

                        BLASMap.insert(std::pair(key, blas));

                        VG_INFO("[Renderer] Create BLAS 0x%016X for instance \"%s\" with key 0x%016X (RefCount=%u)", blas, instance->getName().c_str(), key, blas->getRefCount());
                    }
                }
            }
            m_meshInstanceUpdateQueue.clear();

            // Skins require BLAS update every frame
            const auto & skins = Renderer::get()->getSharedCullingJobOutput()->m_skins;
            if (skins.size() > 0)
            {
                VG_PROFILE_GPU("Skins");

                // D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS: "The memory pointed to must be in state D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE."
                // https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_build_raytracing_acceleration_structure_inputs
                if (nullptr != _skinningBuffer)
                    _cmdList->transitionResource(_skinningBuffer, gfx::ResourceState::UnorderedAccess, gfx::ResourceState::NonPixelShaderResource);
               
                for (uint i = 0; i < skins.size(); ++i)
                {
                    MeshInstance * skin = skins[i];
                    auto * blas = skin->getInstanceBLAS();

                    // Create BLAS collection if it does not exist yet or it's key changed
                    gfx::BLASVariantKey key = skin->computeBLASVariantKey();

                    if (nullptr == blas || blas->getKey() != key)
                    {
                        // Create instance BLAS from model geometry
                        blas = new gfx::BLAS(gfx::BLASUpdateType::Dynamic, key);
                        skin->setInstanceBLAS(blas);
                        blas->Release();
                        VG_INFO("[Renderer] Create BLAS 0x%016X for skinned instance \"%s\" with key 0x%016X (RefCount=%u)", blas, skin->getName().c_str(), key, blas->getRefCount());
                    }

                    blas->clear();

                    const MeshModel * meshModel = skin->getMeshModel(Lod::Lod0);

                    const MeshGeometry * meshGeo = meshModel->getGeometry();
                    gfx::Buffer * ib = meshGeo->getIndexBuffer();
                    const uint ibOffset = meshGeo->getIndexBufferOffset();

                    const gfx::Buffer * modelVB = meshGeo->getVertexBuffer();
                    const gfx::Buffer * skinVB = skin->getSkinnedMeshBuffer();
                    const uint skinVBOffset = skin->getSkinnedMeshBufferOffset();

                    VertexFormat vtxFmt = meshGeo->getVertexFormat();
                    const uint vertexStride = getVertexFormatStride(vtxFmt);

                    const auto batches = meshGeo->batches();
                    const auto & materials = skin->getMaterials();

                    for (uint m = 0; m < batches.size(); ++m)
                    {
                        gfx::SurfaceType surfaceType = gfx::SurfaceType::Opaque;
                        const MaterialModel * mat = m < materials.size() ? materials[m] : nullptr;
                        if (mat)
                            surfaceType = mat->getSurfaceType();

                        const Batch & batch = batches[m];
                        blas->addIndexedGeometry(ib, ibOffset + batch.offset, batch.count, skinVB, skinVBOffset, modelVB->getBufDesc().getElementCount(), vertexStride, surfaceType);
                    }

                    blas->update(_cmdList);
                }

                if (nullptr != _skinningBuffer)
                    _cmdList->transitionResource(_skinningBuffer, gfx::ResourceState::NonPixelShaderResource, gfx::ResourceState::UnorderedAccess);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    // Not only the visible objects have to be added to TLAS, but also the ones visible from 
    // shadow casters if raytraced shadows are used for those lights.
    //--------------------------------------------------------------------------------------
    void RayTracingManager::updateView(gfx::CommandList * _cmdList, View * _view)
    {
        VG_ASSERT(_view);
        if (_view)
        {
            if (_view->IsVisible() && _view->IsUsingRayTracing())
            {
                gfx::TLAS * tlas = _view->getTLAS();
                if (nullptr == tlas)
                {
                    const auto startBuildTLAS = Timer::getTick();
                    tlas = new gfx::TLAS();
                    _view->setTLAS(tlas);
                    VG_INFO("[Renderer] Built TLAS for View \"%s\" in %.2f ms", _view->getName().c_str(), Timer::getEnlapsedTime(startBuildTLAS, Timer::getTick()));
                }

                tlas->reset();

                bool updateTLAS = false;

                const GraphicInstanceList & instances = _view->getCullingJobResult().get(GraphicInstanceListType::All);

                core::set<GraphicInstance *> set;

                for (uint i = 0; i < instances.m_instances.size(); ++i)
                {
                    GraphicInstance * instance = (GraphicInstance *)instances.m_instances[i];
                    set.insert(instance);
                }     

                const auto shadowViews = _view->getShadowViews();
                for (ShadowView * shadowView : shadowViews)
                {
                    const GraphicInstanceList & instances = shadowView->getCullingJobResult().get(GraphicInstanceListType::All);

                    for (uint i = 0; i < instances.m_instances.size(); ++i)
                    {
                        GraphicInstance * instance = (GraphicInstance *)instances.m_instances[i];
                        set.insert(instance);
                    }
                }

                uint index = 0;
                for (GraphicInstance * instance : set)
                {
                    updateTLAS |= instance->OnUpdateRayTracing(_cmdList, _view, index);
                    index++;
                }

                if (updateTLAS)
                    tlas->build(_cmdList);
            }
            else
            {
                _view->setTLAS(nullptr);
            }
        }
    }
}