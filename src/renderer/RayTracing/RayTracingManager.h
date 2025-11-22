#pragma once
#include "core/Object/Object.h"
#include "core/Singleton/Singleton.h"
#include "core/Scheduler/Mutex.h"
#include "core/Container/atomicvector.h"

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

        void    beginFrame              ();

        void    prepareBLAS             ();
        void    updateBLAS              (gfx::CommandList * _cmdList, gfx::Buffer * _skinningBuffer);

        void    prepareTLAS             (View * _view);
        void    updateTLAS              (gfx::CommandList * _cmdList, View * _view);

        bool    enableRayTracing        (bool _enable);
        bool    isRayTracingEnabled     () const;

        void    addMeshModel            (MeshModel * _meshModel);
        void    removeMeshModel         (MeshModel * _meshModel);

        void    addMeshInstance         (MeshInstance * _meshInstance);
        void    removeMeshInstance      (MeshInstance * _meshInstance);
        void    updateMeshInstance      (MeshInstance * _meshInstance);

    private:
        void    onEnableRayTracing      ();
        void    onDisableRayTracing     ();

    private:    
        bool    m_rayTracingRequested   = false;
        bool    m_rayTracingEnabled     = false;

        core::vector<MeshModel *>       m_meshModels;
        core::vector<MeshInstance *>    m_meshInstances;
        core::vector<MeshInstance *>    m_meshInstanceUpdateQueue;
        mutable core::Mutex             m_addRTMeshInstanceMutex = core::Mutex("Mutex - AddRTMeshInstance");

        core::vector<gfx::BLAS *>       m_blasToCreate;
        core::vector<gfx::BLAS *>       m_blasToUpdate;
    };
}