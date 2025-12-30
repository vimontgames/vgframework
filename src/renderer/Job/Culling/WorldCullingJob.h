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
    class GraphicInstance;
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
        // This one does not not atomic because it's filled during serial phase at the beginning
        core::vector<WorldCullingJobOutput>             m_allVisibleWorlds;

        // Using atomics so as to fill multiple worlds at once, but in practice we usually have one big world + small worlds
        // So we could just copy the largest array from worlds, then add the instances from other worlds to this copy instead
        core::atomicvector<GraphicInstance *>           m_allGraphicInstances;
        core::atomicvector<MeshInstance *>              m_allSkinnedMeshInstances;
        core::atomicvector<ParticleSystemInstance *>    m_allParticleSystemInstances;

        SharedWorldCullingJobOutput() :
            m_allGraphicInstances(16384),
            m_allSkinnedMeshInstances(4096),
            m_allParticleSystemInstances(1024)
        {
         
        }

        void clear()
        {
            m_allVisibleWorlds.clear();
            m_allGraphicInstances.clear();
            m_allSkinnedMeshInstances.clear();
            m_allParticleSystemInstances.clear();
        }   

        const WorldCullingJobOutput * getWorldCullingJobOutput(const core::IWorld * _world) const;
        WorldCullingJobOutput * getWorldCullingJobOutput(const core::IWorld * _world);
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

    private:
        const core::IWorld *            m_world         = nullptr;
        WorldCullingJobOutput *         m_output        = nullptr;
        SharedWorldCullingJobOutput *   m_sharedOutput  = nullptr;
    };
}