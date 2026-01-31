#include "Culling/WorldCullingJob.h"

#include "core/GameObject/GameObject.h"
#include "core/string/string.h"
#include "core/IBaseScene.h"
#include "renderer/Renderer.h"
#include "renderer/Instance/Mesh/MeshInstance.h"
#include "renderer/Instance/ParticleSystem/ParticleSystemInstance.h"
#include "renderer/Instance/Light/LightInstance.h"
#include "renderer/Instance/Camera/CameraInstance.h"
#include "renderer/Scene/SceneRenderData.h"

using namespace vg::core;

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    WorldCullingJobOutput::WorldCullingJobOutput(core::IWorld * _world) :
        m_world(_world)
    {
        m_worldCullingJob = new WorldCullingJob("World Culling Job", nullptr, _world, Renderer::get()->getSharedWorldCullingJobOutput());
    }

    //--------------------------------------------------------------------------------------
    WorldCullingJobOutput::WorldCullingJobOutput(WorldCullingJobOutput && _other)
    {
        m_world = _other.m_world;
        m_views = _other.m_views;
        m_staticMeshInstances = _other.m_staticMeshInstances;
        m_skinnedMeshInstances = _other.m_skinnedMeshInstances;
        m_particleSystemInstances = _other.m_particleSystemInstances;
        m_lightInstances = _other.m_lightInstances;
        m_cameraInstances = _other.m_cameraInstances;

        m_worldCullingJob = _other.m_worldCullingJob;
        VG_SAFE_INCREASE_REFCOUNT(m_worldCullingJob);
    }

    //--------------------------------------------------------------------------------------
    WorldCullingJobOutput::~WorldCullingJobOutput()
    {
        VG_SAFE_RELEASE(m_worldCullingJob);
    }

    //--------------------------------------------------------------------------------------
    const WorldCullingJobOutput * SharedWorldCullingJobOutput::getWorldCullingJobOutput(const core::IWorld * _world) const
    {
        for (uint i = 0; i < m_allVisibleWorlds.size(); ++i)
        {
            if (m_allVisibleWorlds[i].m_world == _world)
                return &m_allVisibleWorlds[i];
        }

        VG_ASSERT(false);
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    WorldCullingJobOutput * SharedWorldCullingJobOutput::getWorldCullingJobOutput(const core::IWorld * _world)
    {
        for (uint i = 0; i < m_allVisibleWorlds.size(); ++i)
        {
            if (m_allVisibleWorlds[i].m_world == _world)
                return &m_allVisibleWorlds[i];
        }

        VG_ASSERT(false);
        return nullptr;
    }


    //--------------------------------------------------------------------------------------
    WorldCullingJob::WorldCullingJob(const core::string & _name, core::IObject * _parent, const core::IWorld * _world, SharedWorldCullingJobOutput * const _sharedOutput) :
        Job(_name, _parent)
    {
        VG_ASSERT(_world);
        VG_ASSERT(_sharedOutput);

        m_world = _world;
        m_sharedOutput = _sharedOutput;
    }
    
    //--------------------------------------------------------------------------------------
    void WorldCullingJob::Run()
    {
        m_output = m_sharedOutput->getWorldCullingJobOutput(m_world);

        const string name = fmt::sprintf("Culling \"%s\" World", m_output->m_world->GetName());
        VG_PROFILE_CPU(name); // string

        if (nullptr != m_output)
        {
            for (uint iSceneType = 0; iSceneType < enumCount<BaseSceneType>(); iSceneType++)
            {
                BaseSceneType sceneType = (BaseSceneType)iSceneType;
                if (sceneType != BaseSceneType::Scene)
                    continue;

                const uint sceneCount = m_world->GetSceneCount(sceneType);
                for (uint jScene = 0; jScene < sceneCount; ++jScene)
                {
                    const IBaseScene * scene = m_world->GetScene(jScene, sceneType);
                    SceneRenderData * sceneRenderData = (SceneRenderData*) scene->GetSceneRenderData();
                    VG_ASSERT(sceneRenderData);

                    // Cameras
                    {
                        VG_PROFILE_CPU("Cameras");
                        m_output->m_cameraInstances.reserve(m_output->m_cameraInstances.size() + sceneRenderData->m_cameraInstances.size());
                        for (auto * instance : sceneRenderData->m_cameraInstances)
                        {
                            if (instance->isEnabledInHierarchy()) 
                            {
                                m_output->m_cameraInstances.push_back(instance);
                                m_sharedOutput->m_allGraphicInstances.push_back_atomic((GraphicInstance *)instance);
                            }
                        }
                    }

                    // Lights
                    {
                        VG_PROFILE_CPU("Lights");
                        m_output->m_lightInstances.reserve(m_output->m_lightInstances.size() + sceneRenderData->m_lightInstances.size());
                        for (auto * instance : sceneRenderData->m_lightInstances)
                        {
                            if (instance->isEnabledInHierarchy()) 
                            {
                                m_output->m_lightInstances.push_back(instance);
                                m_sharedOutput->m_allGraphicInstances.push_back_atomic((GraphicInstance *)instance);
                            }
                        }
                    }

                    // Particles
                    {
                        VG_PROFILE_CPU("Particles");
                        m_output->m_particleSystemInstances.reserve(m_output->m_particleSystemInstances.size() + sceneRenderData->m_particleSystemInstances.size());
                        for (auto * instance : sceneRenderData->m_particleSystemInstances)
                        {
                            if (instance->isEnabledInHierarchy()) 
                            {
                                m_output->m_particleSystemInstances.push_back(instance);
                                m_sharedOutput->m_allParticleSystemInstances.push_back_atomic(instance);
                                m_sharedOutput->m_allGraphicInstances.push_back_atomic((GraphicInstance *)instance);
                            }
                        }
                    }

                    // Skinned Meshes
                    {
                        VG_PROFILE_CPU("Skinned");
                        m_output->m_skinnedMeshInstances.reserve(m_output->m_skinnedMeshInstances.size() + sceneRenderData->m_skinnedMeshInstances.size());
                        for (auto * instance : sceneRenderData->m_skinnedMeshInstances)
                        {
                            if (instance->isEnabledInHierarchy()) 
                            {
                                m_output->m_skinnedMeshInstances.push_back(instance);
                                m_sharedOutput->m_allSkinnedMeshInstances.push_back_atomic(instance);
                                m_sharedOutput->m_allGraphicInstances.push_back_atomic((GraphicInstance *)instance);
                            }
                        }
                    }

                    // Static Meshes
                    {
                        VG_PROFILE_CPU("Meshes");
                        m_output->m_staticMeshInstances.reserve(m_output->m_staticMeshInstances.size() + sceneRenderData->m_staticMeshInstances.size());
                        for (auto * instance : sceneRenderData->m_staticMeshInstances)
                        {
                            if (instance->isEnabledInHierarchy()) 
                            {
                                m_output->m_staticMeshInstances.push_back(instance);
                                m_sharedOutput->m_allGraphicInstances.push_back_atomic((GraphicInstance *)instance);
                            }
                        }
                    }
                }
            }                
        }
    }
}
