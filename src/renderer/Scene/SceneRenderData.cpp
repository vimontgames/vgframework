#include "renderer/Precomp.h"
#include "SceneRenderData.h"
#include "renderer/Instance/Mesh/MeshInstance.h"
#include "renderer/Instance/ParticleSystem/ParticleSystemInstance.h"
#include "renderer/Instance/Light/LightInstance.h"
#include "renderer/Instance/Camera/CameraInstance.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    SceneRenderData::SceneRenderData(const core::string & _name, core::IObject * _parent) :
        IGraphicScene(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    SceneRenderData::~SceneRenderData()
    {
        for (auto * instance : m_staticMeshInstances)
            VG_SAFE_RELEASE(instance);
        m_staticMeshInstances.clear();

        for (auto * instance : m_skinnedMeshInstances)
            VG_SAFE_RELEASE(instance);
        m_skinnedMeshInstances.clear();

        for (auto * instance : m_particleSystemInstances)
            VG_SAFE_RELEASE(instance);
        m_particleSystemInstances.clear();

        for (auto * instance : m_lightInstances)
            VG_SAFE_RELEASE(instance);
        m_lightInstances.clear();

        for (auto * instance : m_cameraInstances)
            VG_SAFE_RELEASE(instance);
        m_cameraInstances.clear();
    }

    //--------------------------------------------------------------------------------------
    bool SceneRenderData::RegisterGraphicInstance(IGraphicInstance * _graphicInstance)
    {
        GraphicInstance * instance = VG_SAFE_STATIC_CAST(GraphicInstance, _graphicInstance);

        instance->AddRef();

        bool added = false;

        switch (instance->GetGraphicIntanceType())
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(instance->GetGraphicIntanceType());
                break;

            case GraphicInstanceType::Mesh:
            {
                MeshInstance * meshInstance = (MeshInstance *)instance;

                if (meshInstance->isSkinned())
                    added = vector_helper::addUnique(m_skinnedMeshInstances, meshInstance);
                else
                    added = vector_helper::addUnique(m_staticMeshInstances, meshInstance);
            }
            break;

            case GraphicInstanceType::ParticleSystem:
            {
                added = vector_helper::addUnique(m_particleSystemInstances, (ParticleSystemInstance *)instance);
            }
            break;

            case GraphicInstanceType::Camera:
            {
                added = vector_helper::addUnique(m_cameraInstances, (CameraInstance *)instance);
            }
            break;

            case GraphicInstanceType::Light:
            {
                added = vector_helper::addUnique(m_lightInstances, (LightInstance *)instance);
            }
            break;
        }

        VG_ASSERT(added, "[Renderer] Could not register '%s' graphic instance \"%s\" to world \"%s\"", asCString(instance->GetGraphicIntanceType()), _graphicInstance->GetName().c_str(), GetParent()->GetName().c_str());
        return added;
    }

    //--------------------------------------------------------------------------------------
    bool SceneRenderData::UnregisterGraphicInstance(IGraphicInstance * _graphicInstance)
    {
        GraphicInstance * instance = VG_SAFE_STATIC_CAST(GraphicInstance, _graphicInstance);

        bool removed = false;

        switch (instance->GetGraphicIntanceType())
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(instance->GetGraphicIntanceType());
                break;

            case GraphicInstanceType::Mesh:
            {
                MeshInstance * meshInstance = (MeshInstance *)instance;

                if (meshInstance->isSkinned())
                    removed = vector_helper::remove(m_skinnedMeshInstances, meshInstance);
                else
                    removed = vector_helper::remove(m_staticMeshInstances, meshInstance);
            }
            break;

            case GraphicInstanceType::ParticleSystem:
            {
                removed = vector_helper::remove(m_particleSystemInstances, (ParticleSystemInstance *)instance);
            }
            break;

            case GraphicInstanceType::Camera:
            {
                removed = vector_helper::remove(m_cameraInstances, (CameraInstance *)instance);
            }
            break;

            case GraphicInstanceType::Light:
            {
                removed = vector_helper::remove(m_lightInstances, (LightInstance *)instance);
            }
            break;
        }

        VG_ASSERT(removed, "[Renderer] Could not unregister '%s' graphic instance \"%s\" from world \"%s\"", asCString(instance->GetGraphicIntanceType()), _graphicInstance->GetFullName().c_str(), GetParent()->GetName().c_str());
        if (removed)
        {
            instance->SetParent(nullptr);
            VG_SAFE_RELEASE(instance);
        }
        return removed;
    }
}