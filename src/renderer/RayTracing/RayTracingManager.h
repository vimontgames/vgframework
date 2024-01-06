#pragma once
#include "core/Object/Object.h"
#include "core/Singleton/Singleton.h"

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

        void    update                  (gfx::CommandList * _cmdList, gfx::Buffer * _skinningBuffer);
        void    updateView              (gfx::CommandList * _cmdList, View * _view);

        bool    enableRayTracing        (bool _enable);
        bool    isRayTracingEnabled     () const;

        void    addMeshModel            (MeshModel * _meshModel);
        void    removeMeshModel         (MeshModel * _meshModel);
        //void    updateMeshModel         (MeshModel * _meshModel);

        void    addMeshInstance         (MeshInstance * _meshInstance);
        void    removeMeshInstance      (MeshInstance * _meshInstance);
        void    updateMeshInstance      (MeshInstance * _meshInstance);

    private:
        void    onEnableRayTracing      ();
        void    onDisableRayTracing     ();
        //void    createMeshModelBLAS     (MeshModel * _meshModel);

    private:    
        bool    m_rayTracingEnabled = false;

        core::vector<MeshModel *>       m_meshModels;
        core::vector<MeshInstance *>    m_meshInstances;
        core::vector<MeshInstance *>    m_meshInstanceUpdateQueue;
        //core::vector<MeshModel*>        m_meshModelUpdateQueue;
    };
}