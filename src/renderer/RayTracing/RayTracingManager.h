#pragma once
#include "core/Object/Object.h"
#include "core/Singleton/Singleton.h"
#include "core/Scheduler/Mutex.h"
#include "core/Container/Span.h"

namespace vg::gfx
{
    class CommandList;
    class Buffer;
    class BLAS;
}

namespace vg::renderer
{
    class MeshModel;
    class MeshInstance;
    class View;
    class GraphicInstance;

    class RayTracingManager final : public core::Object, public core::Singleton<RayTracingManager>
    {
    public:
        RayTracingManager();
        ~RayTracingManager();

        void                            beginFrame              ();

        void                            prepareBLAS             ();
        void                            updateBLAS              (gfx::CommandList * _cmdList, gfx::Buffer * _skinningBuffer);

        void                            prepareTLAS             (View * _view);
        void                            updateTLAS              (gfx::CommandList * _cmdList, View * _view);

        void                            addMeshModel            (MeshModel * _meshModel);
        void                            removeMeshModel         (MeshModel * _meshModel);

        void                            addMeshInstance         (MeshInstance * _meshInstance);
        void                            removeMeshInstance      (MeshInstance * _meshInstance);
        void                            updateMeshInstance      (MeshInstance * _meshInstance);

        void                            onEnableRayTracing      ();
        void                            onDisableRayTracing     ();

        core::span<MeshInstance *>      getSkinnedMeshInstances () const;

    private:    

        core::vector<MeshModel *>       m_meshModels;
        core::vector<MeshInstance *>    m_meshInstances; 

        core::vector<MeshInstance *>    m_meshInstanceUpdateQueue;
        core::vector<MeshInstance *>    m_skinnedMeshUpdateQueue;

        mutable core::Mutex             m_addRTMeshInstanceMutex = core::Mutex("Mutex - AddRTMeshInstance");

        core::vector<gfx::BLAS *>       m_blasToCreate;
        core::vector<gfx::BLAS *>       m_blasToUpdate;
    };
}