#pragma once

#include "core/Scheduler/Job.h"
#include "core/Container/AtomicVector.h"

namespace vg::core
{
    class IWorld;
    class GameObject;
}

namespace vg::renderer
{
    class WorldCullingJob;
    class IView;
    class MeshInstance;
    class ParticleSystemInstance;
    class LightInstance;
    class CameraInstance;

    struct WorldCullingJobOutput
    {
        WorldCullingJobOutput(core::IWorld * _world);
        WorldCullingJobOutput(WorldCullingJobOutput && _other);
        ~WorldCullingJobOutput();

        WorldCullingJob * getCullingJob() const 
        {
            return m_worldCullingJob;
        }

        void clear()
        {
            m_views.clear();
            m_staticMeshInstances.clear();
            m_skinnedMeshInstances.clear();
            m_particleSystemInstances.clear();
            m_lightInstances.clear();
            m_cameraInstances.clear();
        }

        core::IWorld *                          m_world;
        core::vector<IView *>                   m_views;
        core::vector<MeshInstance *>            m_staticMeshInstances;
        core::vector<MeshInstance *>            m_skinnedMeshInstances;
        core::vector<ParticleSystemInstance *>  m_particleSystemInstances;
        core::vector<LightInstance *>           m_lightInstances;
        core::vector<CameraInstance *>          m_cameraInstances;

        WorldCullingJob *                       m_worldCullingJob = nullptr;
    };

    struct SharedWorldCullingJobOutput
    {
        core::vector<WorldCullingJobOutput>     m_allVisibleWorlds;
        core::atomicvector<MeshInstance *>      m_allVisibleSkinnedMeshInstances;

        SharedWorldCullingJobOutput() :
            m_allVisibleSkinnedMeshInstances(4096)
        {
         
        }

        void clear()
        {
            m_allVisibleWorlds.clear();
            m_allVisibleSkinnedMeshInstances.clear();
        }       
    };

    struct WorldCullingOptions
    {
        core::IWorld * m_world = nullptr;
    };

    class WorldCullingJob : public core::Job
    {
    public:
        const char * GetClassName() const final { return "WorldCullingJob"; }

        WorldCullingJob(const core::string & _name, core::IObject * _parent, const core::IWorld * _world, SharedWorldCullingJobOutput * const _sharedOutput);
        void Run() override;

    protected:
        void cullWorldGameObjectRecur(const core::GameObject * _gameobject);

    private:
        const core::IWorld *            m_world         = nullptr;
        WorldCullingJobOutput *         m_output        = nullptr;
        SharedWorldCullingJobOutput *   m_sharedOutput  = nullptr;
    };
}