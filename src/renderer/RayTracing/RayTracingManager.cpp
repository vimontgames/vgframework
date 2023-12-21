#include "renderer/Precomp.h"
#include "RayTracingManager.h"
#include "core/Timer/Timer.h"
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
    void RayTracingManager::onEnableRayTracing()
    {
        VG_INFO("[Renderer] RayTracing is enabled");

        // Create BLAS for mesh models
        for (uint i = 0; i < m_meshModels.size(); ++i)
        {
            MeshModel * meshModel = m_meshModels[i];
            
            if (!meshModel->getBLAS())
            {
                // Create BLAS from model geometry
                auto blas = new gfx::BLAS();

                const MeshGeometry * meshGeo = meshModel->getGeometry();
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
                    blas->addIndexedGeometry(ib, ibOffset + batch.offset, batch.count, vb, vbOffset, vertexStride);
                }               

                blas->init();

                meshModel->setBLAS(blas);

                updateMeshModel(meshModel);
            }
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
    void RayTracingManager::update(gfx::CommandList * _cmdList)
    {
        if (isRayTracingEnabled())
        {
            while (m_meshModelUpdateQueue.size() > 0)
            {
                MeshModel * meshModel = m_meshModelUpdateQueue[m_meshModelUpdateQueue.size() - 1];
                m_meshModelUpdateQueue.pop_back();

                gfx::BLAS * blas = meshModel->getBLAS();
                VG_ASSERT(blas);
                if (blas)
                {
                    const auto startBuildBLAS = Timer::getTick();
                    blas->build(_cmdList);
                    VG_INFO("[Renderer] Built BLAS for meshModel \"%s\" in %.2f ms", meshModel->getName().c_str(), Timer::getEnlapsedTime(startBuildBLAS, Timer::getTick()));
                }
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

                const GraphicInstanceList & instances = _view->m_cullingJobResult.m_instanceLists[asInteger(GraphicInstanceListType::Opaque)];
                tlas->reset();

                for (uint i = 0; i < instances.m_instances.size(); ++i)
                {
                    VG_ASSERT(dynamic_cast<const MeshInstance *>(instances.m_instances[i]));
                    MeshInstance * meshInstance = (MeshInstance *)instances.m_instances[i];

                    MeshModel * meshModel = (MeshModel *)meshInstance->getModel(Lod::Lod0);
                    const gfx::BLAS * blas = meshModel->getBLAS();
                    tlas->addInstance(blas, meshInstance->getWorldMatrix(), i);
                }

                tlas->build(_cmdList);
            }
            else
            {
                _view->setTLAS(nullptr);
            }
        }
    }
}