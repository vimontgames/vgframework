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
#include "renderer/Instance/Mesh/MeshInstance.h"

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
    void RayTracingManager::createMeshModelBLAS(MeshModel * _meshModel)
    {
        // Create BLAS from model geometry
        auto blas = new gfx::BLAS(gfx::BLASUpdateType::Static);

        const MeshGeometry * meshGeo = _meshModel->getGeometry();
        gfx::Buffer * ib = meshGeo->getIndexBuffer();
        const uint ibOffset = meshGeo->getIndexBufferOffset();

        gfx::Buffer * vb = meshGeo->getVertexBuffer();
        const uint vbOffset = meshGeo->getVertexBufferOffset();

        VertexFormat vtxFmt = meshGeo->getVertexFormat();
        const uint vertexStride = getVertexFormatStride(vtxFmt);

        const auto batches = meshGeo->batches();
        for (uint j = 0; j < batches.size(); ++j)
        {
            const Batch & batch = batches[j];
            blas->addIndexedGeometry(ib, ibOffset + batch.offset, batch.count, vb, vbOffset, vb->getBufDesc().getElementCount(), vertexStride);
        }

        blas->init();

        _meshModel->setBLAS(blas);

        updateMeshModel(_meshModel);
    }

    //--------------------------------------------------------------------------------------
    void RayTracingManager::onEnableRayTracing()
    {
        VG_INFO("[Renderer] RayTracing is enabled");

        // Create BLAS for mesh models
        for (uint i = 0; i < m_meshModels.size(); ++i)
        {
            MeshModel * meshModel = m_meshModels[i];
            
            if (!meshModel->getBLAS())
                createMeshModelBLAS(meshModel);
        }
    }

    //--------------------------------------------------------------------------------------
    void RayTracingManager::onDisableRayTracing()
    {
        VG_INFO("[Renderer] RayTracing is disabled");

        for (uint i = 0; i < m_meshModels.size(); ++i)
        {
            MeshModel * meshModel = m_meshModels[i];
            auto blas = meshModel->getBLAS();
            if (blas)
                meshModel->setBLAS(nullptr);
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

        if (isRayTracingEnabled())
            updateMeshModel(_meshModel);
    }

    //--------------------------------------------------------------------------------------
    void RayTracingManager::updateMeshModel(MeshModel * _meshModel)
    {
        VG_ASSERT(!m_meshModelUpdateQueue.exists(_meshModel));
        m_meshModelUpdateQueue.push_back(_meshModel);
    }

    //--------------------------------------------------------------------------------------
    void RayTracingManager::removeMeshModel(MeshModel * _meshModel)
    {
        VG_ASSERT(m_meshModels.exists(_meshModel));
        m_meshModels.remove(_meshModel);
    }

    //--------------------------------------------------------------------------------------
    void RayTracingManager::update(gfx::CommandList * _cmdList, gfx::Buffer * _skinningBuffer)
    {
        if (isRayTracingEnabled())
        {
            // Static mesh models' BLAS are added right after loading and computed here
            {
                VG_PROFILE_GPU("Models");
                while (m_meshModelUpdateQueue.size() > 0)
                {
                    MeshModel * meshModel = m_meshModelUpdateQueue[m_meshModelUpdateQueue.size() - 1];
                    m_meshModelUpdateQueue.pop_back();

                    gfx::BLAS * blas = meshModel->getBLAS();
                    if (!blas)
                        createMeshModelBLAS(meshModel);

                    if (blas)
                    {
                        const auto startBuildBLAS = Timer::getTick();
                        blas->build(_cmdList);
                        VG_INFO("[Renderer] Built BLAS for meshModel \"%s\" in %.2f ms", meshModel->getName().c_str(), Timer::getEnlapsedTime(startBuildBLAS, Timer::getTick()));
                    }
                }
            }

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
                    bool update = true;
                    if (nullptr == blas)
                    {
                        // Create instance BLAS from model geometry
                        blas = new gfx::BLAS(gfx::BLASUpdateType::Dynamic);
                        skin->setInstanceBLAS(blas);
                        update = false;
                    }

                    blas->clear();

                    const MeshModel * meshModel = skin->getMeshModel(Lod::Lod0);

                    const MeshGeometry * meshGeo = meshModel->getGeometry();
                    gfx::Buffer * ib = meshGeo->getIndexBuffer();
                    const uint ibOffset = meshGeo->getIndexBufferOffset();

                    const gfx::Buffer * modelvb = meshGeo->getVertexBuffer();
                    const gfx::Buffer * skinvb = skin->getSkinnedMeshBuffer();
                    const uint vbOffset = skin->getSkinnedMeshBufferOffset();

                    VertexFormat vtxFmt = meshGeo->getVertexFormat();
                    const uint vertexStride = getVertexFormatStride(vtxFmt);

                    const auto batches = meshGeo->batches();
                    for (uint j = 0; j < batches.size(); ++j)
                    {
                        const Batch & batch = batches[j];
                        blas->addIndexedGeometry(ib, ibOffset + batch.offset, batch.count, skinvb, vbOffset, modelvb->getBufDesc().getElementCount(), vertexStride);
                    }

                    blas->update(_cmdList); 
                }

                if (nullptr != _skinningBuffer)
                    _cmdList->transitionResource(_skinningBuffer, gfx::ResourceState::NonPixelShaderResource, gfx::ResourceState::UnorderedAccess);
            }
        }
    }

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

                const GraphicInstanceList & instances = _view->getCullingJobResult().get(GraphicInstanceListType::Opaque);
                tlas->reset();

                bool updateTLAS = false;
                for (uint i = 0; i < instances.m_instances.size(); ++i)
                {
                    GraphicInstance * instance = (GraphicInstance*)instances.m_instances[i];
                    updateTLAS |= instance->OnUpdateRayTracing(_cmdList, _view, i);
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