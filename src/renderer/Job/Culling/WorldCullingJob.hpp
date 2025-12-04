#include "Culling/WorldCullingJob.h"

#include "core/GameObject/GameObject.h"
#include "core/string/string.h"
#include "core/IBaseScene.h"
#include "renderer/Renderer.h"
#include "renderer/Instance/Mesh/MeshInstance.h"

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
    WorldCullingJob::WorldCullingJob(const core::string & _name, core::IObject * _parent, const core::IWorld * _world, SharedWorldCullingJobOutput * const _sharedOutput) :
        Job(_name, _parent)
    {
        VG_ASSERT(_world);
        VG_ASSERT(_sharedOutput);

        m_world = _world;
        m_sharedOutput = _sharedOutput;
    }

    //--------------------------------------------------------------------------------------
    void WorldCullingJob::cullWorldGameObjectRecur(const core::GameObject * _gameobject)
    {
        // Visible? Then add it to the list
        if (asBool(InstanceFlags::Enabled & _gameobject->getInstanceFlags()))
        {
            auto & instances = _gameobject->getGraphicInstances();
            const auto instanceCount = instances.size();
            for (uint i = 0; i < instanceCount; ++i)
            {
                GraphicInstance * instance = (GraphicInstance *)instances[i];
        
                if (asBool(InstanceFlags::Enabled & instance->getInstanceFlags()))
                {
                    switch (instance->GetGraphicIntanceType())
                    {
                        default:
                            VG_ASSERT_ENUM_NOT_IMPLEMENTED(instance->GetGraphicIntanceType());
                            break;

                        case GraphicInstanceType::Mesh:
                        {
                            MeshInstance * meshInstance = (MeshInstance *)instance;
                            if (meshInstance->isSkinned())
                            {
                                m_output->m_skinnedMeshInstances.push_back(meshInstance);
                                m_sharedOutput->m_allVisibleSkinnedMeshInstances.push_back_atomic(meshInstance);
                            }
                            else
                                m_output->m_staticMeshInstances.push_back(meshInstance);
                        }
                        break;

                        case GraphicInstanceType::ParticleSystem:
                        {
                            m_output->m_particleSystemInstances.push_back((ParticleSystemInstance *)instance);
                        }
                        break;

                        case GraphicInstanceType::Camera:
                        {
                            m_output->m_cameraInstances.push_back((CameraInstance *)instance);
                        }
                        break;

                        case GraphicInstanceType::Light:
                        {
                            m_output->m_lightInstances.push_back((LightInstance *)instance);
                        }
                        break;

                    }
                }
            }
        
            const auto & children = _gameobject->GetChildren();
            const auto childCount = children.size();
            for (uint i = 0; i < childCount; ++i)
            {
                const GameObject * child = VG_SAFE_STATIC_CAST(GameObject, children[i]);
                cullWorldGameObjectRecur(child);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void WorldCullingJob::Run()
    {
        m_output = nullptr;
        for (uint i = 0; i < m_sharedOutput->m_allVisibleWorlds.size(); ++i)
        {
            if (m_sharedOutput->m_allVisibleWorlds[i].m_world == m_world)
            {
                m_output = &m_sharedOutput->m_allVisibleWorlds[i];
                break;
            }
        }

        const string name = fmt::sprintf("Culling \"%s\" World", m_output->m_world->GetName());
        VG_PROFILE_CPU(name.c_str());

        if (nullptr != m_output)
        {
            const uint count = m_output->m_world->GetSceneCount(BaseSceneType::Scene);
            for (uint i = 0; i < count; ++i)
            {
                const auto * scene = m_output->m_world->GetScene(i, BaseSceneType::Scene);
                const auto * root = (GameObject *)scene->GetRoot();
                VG_ASSERT("[Culling] Scene has no root node");
                if (root)
                    cullWorldGameObjectRecur(root);
            }
        }
    }
}
