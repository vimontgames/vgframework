#pragma once
#include "core/Object/Object.h"
#include "core/Singleton/Singleton.h"
#include "core/Scheduler/Mutex.h"

namespace vg::gfx
{
    class CommandList;
    class Buffer;
}

namespace vg::renderer
{
    class MeshModel;
    class MeshInstance;
    class View;

    class RayTracingManager final : public core::Object, public core::Singleton<RayTracingManager>
    {
    public:

        void    beginFrame              ();

        void    update                  (gfx::CommandList * _cmdList, gfx::Buffer * _skinningBuffer);
        void    updateView              (gfx::CommandList * _cmdList, View * _view);

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
    };
}