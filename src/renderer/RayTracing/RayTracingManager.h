#pragma once
#include "core/Object/Object.h"
#include "core/Singleton/Singleton.h"

namespace vg::gfx
{
    class CommandList;
}

namespace vg::renderer
{
    class MeshModel;
    class MeshInstance;
    class View;

    class RayTracingManager final : public core::Object, public core::Singleton<RayTracingManager>
    {
    public:

        void    update                  (gfx::CommandList * _cmdList);
        void    updateView              (gfx::CommandList * _cmdList, View * _view);

        bool    enableRayTracing        (bool _enable);
        bool    isRayTracingEnabled     () const;

        void    addMeshModel            (MeshModel * _meshModel);
        void    removeMeshModel         (MeshModel * _meshModel);
        void    updateMeshModel         (MeshModel * _meshModel);

    private:
        void    onEnableRayTracing      ();
        void    onDisableRayTracing     ();

    private:    
        bool    m_rayTracingEnabled = false;

        core::vector<MeshModel *>       m_meshModels;
        core::vector<MeshModel*>        m_meshModelUpdateQueue;
    };
}