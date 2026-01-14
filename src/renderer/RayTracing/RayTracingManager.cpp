#include "renderer/Precomp.h"
#include "RayTracingManager.h"
#include "core/Timer/Timer.h"
#include "core/Scheduler/Scheduler.h"
#include "core/Container/Span.h"
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
#include "renderer/Options/RendererOptions.h"
#include "renderer/Job/Culling/WorldCullingJob.h"

using namespace vg::core;
using namespace vg::gfx;

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    RayTracingManager::RayTracingManager()
    {

    }

    //--------------------------------------------------------------------------------------
    RayTracingManager::~RayTracingManager()
    {

    }

    //--------------------------------------------------------------------------------------
    // Can't create BLAS right after enabling RayTracing because the actual BLASes to use 
    // will depend on the materials' surface types, so instead we create a BLASCollection 
    // hash that will be populated on-demand.
    //--------------------------------------------------------------------------------------
    void RayTracingManager::onEnableRayTracing()
    {
        lock_guard lock(m_addRTMeshInstanceMutex);
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
        lock_guard lock(m_addRTMeshInstanceMutex);
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
        VG_ASSERT(!vector_helper::exists(m_meshModels, _meshModel));
        m_meshModels.push_back(_meshModel);
    }
    
    //--------------------------------------------------------------------------------------
    void RayTracingManager::removeMeshModel(MeshModel * _meshModel)
    {
        VG_ASSERT(vector_helper::exists(m_meshModels, _meshModel));
        vector_helper::remove(m_meshModels, _meshModel);
    }

    //--------------------------------------------------------------------------------------
    void RayTracingManager::addMeshInstance(MeshInstance * _meshInstance)
    {
        lock_guard lock(m_addRTMeshInstanceMutex);
        VG_ASSERT(!vector_helper::exists(m_meshInstances, _meshInstance));
        m_meshInstances.push_back(_meshInstance);        
    }

    //--------------------------------------------------------------------------------------
    void RayTracingManager::removeMeshInstance(MeshInstance * _meshInstance)
    {
        lock_guard lock(m_addRTMeshInstanceMutex);
        VG_ASSERT(vector_helper::exists(m_meshInstances, _meshInstance));
        vector_helper::remove(m_meshInstances, _meshInstance);

        // Temp: Why do we need that update queue when we could use the shared world culling result? Just need to create m_staticMeshes (View culling) and m_allStaticMeshInstances (World culling) lists
        if (vector_helper::exists(m_skinnedMeshUpdateQueue, _meshInstance))
            vector_helper::remove(m_skinnedMeshUpdateQueue, _meshInstance);
        
        if(vector_helper::exists(m_meshInstanceUpdateQueue, _meshInstance))
            vector_helper::remove(m_meshInstanceUpdateQueue, _meshInstance);
    }

    //--------------------------------------------------------------------------------------
    // TODO : Use 'DirtyBLAS' + atomic to avoid search? That list could get cleared before 
    // the end of the frame because we keep track of the full instance list.
    //--------------------------------------------------------------------------------------
    void RayTracingManager::updateMeshInstance(MeshInstance * _meshInstance)
    {
        if (_meshInstance->IsSkinned())
        {
            if (!vector_helper::exists(m_skinnedMeshUpdateQueue, _meshInstance))
                m_skinnedMeshUpdateQueue.push_back(_meshInstance);
        }
        else
        {
            if (!vector_helper::exists(m_meshInstanceUpdateQueue, _meshInstance))
                m_meshInstanceUpdateQueue.push_back(_meshInstance);
        }
    }

    //--------------------------------------------------------------------------------------
    void RayTracingManager::beginFrame()
    {
        
    }

    //--------------------------------------------------------------------------------------
    core::span<MeshInstance *> RayTracingManager::getSkinnedMeshInstances() const
    {
        const auto & options = RendererOptions::get();
        if (options->getRayTracingTLASMode() == TLASMode::PerView)
            return Renderer::get()->getSharedCullingJobOutput()->m_skins;
        else
            return Renderer::get()->getSharedWorldCullingJobOutput()->m_allSkinnedMeshInstances;
    }
  
    //--------------------------------------------------------------------------------------
    // Prepare BLAS to create and update
    //--------------------------------------------------------------------------------------
    void RayTracingManager::prepareBLAS()
    {
        VG_ASSERT(RendererOptions::get()->isRayTracingEnabled());
        VG_PROFILE_CPU("Prepare BLAS");
        
        // Non-skins require update when model or material surface types change, but BLAS can be shared
        if (m_meshInstanceUpdateQueue.size() > 0)
        {
            VG_PROFILE_CPU("Static");
            for (uint i = 0; i < m_meshInstanceUpdateQueue.size(); ++i)
            {
                MeshInstance * instance = m_meshInstanceUpdateQueue[i];
                VG_ASSERT(!instance->IsSkinned());
                auto * blas = instance->getInstanceBLAS();
                MeshModel * meshModel = (MeshModel *)instance->getMeshModel(Lod::Lod0);
                //VG_ASSERT(nullptr != meshModel); // Can happen during hot reload
                if (nullptr == meshModel)
                    continue;

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
                        const auto & batchMask = instance->getBatchMask();

                        for (uint m = 0; m < batches.size(); ++m)
                        {
                            SurfaceType surfaceType = SurfaceType::Opaque;

                            if (batchMask.getBitValue(m))
                            {
                                const MaterialModel * mat = m < materials.size() ? materials[m] : nullptr;
                                if (mat)
                                    surfaceType = mat->GetSurfaceType();
                            }
                            else
                            {
                                surfaceType = SurfaceType::AlphaBlend;
                            }

                            const Batch & batch = batches[m];
                            VG_ASSERT(blas);
                            blas->addIndexedGeometry(ib, ibOffset, batch.offset, batch.count, modelVB, 0, modelVB->getBufDesc().getElementCount(), vertexStride, (SurfaceType::Opaque == surfaceType) ? true : false);
                        }

                        BLASMap.insert(std::pair(key, blas));
                        VG_SAFE_INCREASE_REFCOUNT(blas);

                        if (blas->init())
                        {
                            VG_INFO("[Renderer] Create static BLAS 0x%016llX for instance \"%s\" with key 0x%016llX", blas, instance->GetName().c_str(), key);
                            m_blasToCreate.push_back(blas);
                        }
                        else
                        {
                            //VG_INFO("[Renderer] Update static BLAS 0x%016llX for instance \"%s\" with key 0x%016llX", blas, instance->GetName().c_str(), key);
                            m_blasToUpdate.push_back(blas);
                        }
                    }
                }
            }
        }

        // Skins require BLAS update every frame (TODO: only skins from visible worlds?)
        const auto & skins = getSkinnedMeshInstances();

        if (skins.size() > 0)
        {
            VG_PROFILE_CPU("Skins");

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
                }

                blas->clear();

                const MeshModel * meshModel = skin->getMeshModel(Lod::Lod0);

                const MeshGeometry * meshGeo = meshModel->getGeometry();
                gfx::Buffer * ib = meshGeo->getIndexBuffer();
                const uint ibOffset = meshGeo->getIndexBufferOffset();

                const gfx::Buffer * modelVB = meshGeo->getVertexBuffer();
                const gfx::Buffer * skinVB = skin->getInstanceVertexBuffer();
                const uint skinVBOffset = skin->getInstanceVertexBufferOffset();

                VertexFormat vtxFmt = meshGeo->getVertexFormat();
                const uint vertexStride = getVertexFormatStride(vtxFmt);

                const auto batches = meshGeo->batches();
                const auto & materials = skin->getMaterials();
                const auto & batchMask = skin->getBatchMask();

                for (uint m = 0; m < batches.size(); ++m)
                {
                    SurfaceType surfaceType = SurfaceType::Opaque;

                    if (batchMask.getBitValue(m))
                    {
                        const MaterialModel * mat = m < materials.size() ? materials[m] : nullptr;
                        if (mat)
                            surfaceType = mat->GetSurfaceType();
                    }
                    else
                    {
                        surfaceType = SurfaceType::AlphaBlend;
                    }

                    const Batch & batch = batches[m];
                    blas->addIndexedGeometry(ib, ibOffset, batch.offset, batch.count, skinVB, skinVBOffset, modelVB->getBufDesc().getElementCount(), vertexStride, (SurfaceType::Opaque == surfaceType) ? true : false);
                }

                //VG_INFO("[Renderer] Update BLAS 0x%016llX for instance \"%s\" with key 0x%016llX", blas, skin->GetName().c_str(), key);
                if (blas->init())
                {
                    VG_INFO("[Renderer] Create skin BLAS 0x%016llX for instance \"%s\" with key 0x%016llX", blas, skin->GetName().c_str(), key);
                    m_blasToCreate.push_back(blas);
                }
                else
                {
                    //VG_INFO("[Renderer] Update skin BLAS 0x%016llX for instance \"%s\" with key 0x%016llX", blas, skin->GetName().c_str(), key);
                    m_blasToUpdate.push_back(blas);
                }
            }

            // TODO: primitives for quad particle
        }
    }

    //--------------------------------------------------------------------------------------
    // Update is not a right time to create BLAS, because an instance can belong to several view
    // Instead, during culling instances that have no BLAS should be added to a list of instances that need to allocate BLAS
    //--------------------------------------------------------------------------------------
    void RayTracingManager::updateBLAS(gfx::CommandList * _cmdList, gfx::Buffer * _skinningBuffer)
    {
        VG_PROFILE_GPU("Update BLAS");

        // D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS: "The memory pointed to must be in state D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE."
        // https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_build_raytracing_acceleration_structure_inputs
        if (nullptr != _skinningBuffer)
            _cmdList->transitionResource(_skinningBuffer, gfx::ResourceState::UnorderedAccess, gfx::ResourceState::NonPixelShaderResource);

        for (uint i = 0; i < m_blasToCreate.size(); ++i)
        {
            gfx::BLAS * blas = m_blasToCreate[i];
            blas->build(_cmdList);
        }
        m_blasToCreate.clear();

        for (uint i = 0; i < m_blasToUpdate.size(); ++i)
        {
            gfx::BLAS * blas = m_blasToUpdate[i];
            blas->update(_cmdList);
        }
        m_blasToUpdate.clear();

        if (nullptr != _skinningBuffer)
            _cmdList->transitionResource(_skinningBuffer, gfx::ResourceState::NonPixelShaderResource, gfx::ResourceState::UnorderedAccess);
    }

    //--------------------------------------------------------------------------------------
    // Create the TLAS for the view and populate with GraphicInstances:
    // Not only the visible objects have to be added to TLAS, but also the ones visible from 
    // shadow casters if raytraced shadows are used for those lights.
    //--------------------------------------------------------------------------------------
    void RayTracingManager::prepareTLAS(View * _view)
    {
        VG_PROFILE_CPU("Prepare View TLAS");
        VG_ASSERT(_view && _view->IsVisible() && _view->IsUsingRayTracing());

        if (_view->getTLAS() && _view->getTLASMode() != TLASMode::PerView)
            _view->setTLAS(nullptr);

        TLAS * tlas = _view->getTLAS();

        if (nullptr == tlas)
        {
            tlas = new TLAS();
            _view->setTLAS(tlas, TLASMode::PerView);
            tlas->Release();
        }

        tlas->reset();
    }

    //--------------------------------------------------------------------------------------
    void RayTracingManager::prepareTLAS(const core::IWorld * _world, core::vector<View *> & _views)
    {
        VG_PROFILE_CPU("Prepare World TLAS");
        VG_ASSERT(_world);
        VG_ASSERT(_views.size() >= 1);

        // Release invalid TLAS
        for (uint v = 0; v < _views.size(); ++v)
        { 
            auto & view = _views[v];
            if (view->getTLAS() && view->getTLASMode() == TLASMode::PerView)
                view->setTLAS(nullptr);
        }

        // find shared TLAS
        TLAS * tlas = nullptr;
        for (uint v = 0; v < _views.size(); ++v)
        {
            auto & view = _views[v];

            if (view->getTLAS())
            {
                tlas = view->getTLAS();
                break;
            }
        }

        // create new TLAS for world if needed
        bool justCreated = false;
        if (nullptr == tlas)
        {
            tlas = new TLAS();
            justCreated = true;
        }

        // ensure all views of the world share the same TLAS
        for (uint v = 0; v < _views.size(); ++v)
        {
            auto & view = _views[v];

            VG_ASSERT(view->getTLAS() == nullptr || view->getTLAS() == tlas);

            if (!view->getTLAS() || view->getTLAS() != tlas)
                view->setTLAS(tlas, TLASMode::PerWorld);
        }     

        tlas->reset();

        if (justCreated)
            VG_SAFE_RELEASE(tlas);
    }

    //--------------------------------------------------------------------------------------
    void RayTracingManager::updateTLAS(gfx::CommandList * _cmdList, const core::IWorld * _world, core::vector<View *> & _views)
    {
        VG_PROFILE_GPU("Update World TLAS");
        VG_ASSERT(_world);

        // find shared TLAS
        TLAS * tlas = nullptr;
        for (uint v = 0; v < _views.size(); ++v)
        {
            auto & view = _views[v];

            if (view->getTLAS())
            {
                tlas = view->getTLAS();
                break;
            }
        }
        VG_ASSERT(tlas);

        const SharedWorldCullingJobOutput * sharedWorldCulling = Renderer::get()->getSharedWorldCullingJobOutput();
        const WorldCullingJobOutput * worldCulling = sharedWorldCulling->getWorldCullingJobOutput(_world);
        VG_ASSERT(worldCulling);

        bool updateTLAS = false;

        for (GraphicInstance * instance : worldCulling->m_staticMeshInstances)
            updateTLAS |= instance->UpdateTLAS(_cmdList, tlas);

        for (GraphicInstance * instance : worldCulling->m_skinnedMeshInstances)
            updateTLAS |= instance->UpdateTLAS(_cmdList, tlas);

        if (updateTLAS)
            tlas->build(_cmdList);
    }

    //--------------------------------------------------------------------------------------
    // Build the TLAS on the GPU
    // TODO: instead of using a set to eliminate duplicates, add instance directly to TLAS during culling using an atomic flag 
    //--------------------------------------------------------------------------------------
    void RayTracingManager::updateTLAS(gfx::CommandList * _cmdList, View * _view)
    {
        VG_PROFILE_GPU("Update View TLAS");
        VG_ASSERT(_view && _view->IsVisible() && _view->IsUsingRayTracing());

        gfx::TLAS * tlas = _view->getTLAS();
        VG_ASSERT(tlas);
                                
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

        bool updateTLAS = false;
                
        for (GraphicInstance * instance : set)
            updateTLAS |= instance->UpdateTLAS(_cmdList, tlas);
                
        if (updateTLAS)
            tlas->build(_cmdList);
    }
}