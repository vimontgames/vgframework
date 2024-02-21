#include "engine/Precomp.h"
#include "MeshComponent.h"
#include "core/GameObject/GameObject.h"
#include "renderer/IMeshInstance.h"
#include "renderer/IMeshModel.h"
#include "engine/Engine.h"
#include "engine/EngineOptions.h"
#include "renderer/IRenderer.h"
#include "renderer/IPicking.h"
#include "editor/Editor_Consts.h"
#include "core/Scheduler/Scheduler.h"
#include "engine/Job/Animation/AnimationJob.h"
#include "engine/Job/Animation/DrawSkeletonJob.h"

#include "MaterialResourceList.hpp"

#if !VG_ENABLE_INLINE
#include "MeshComponent.inl"
#endif

using namespace vg::core;
using namespace vg::renderer;

namespace vg::engine
{
    VG_REGISTER_COMPONENT_CLASS(MeshComponent, "Mesh", "Rendering", "Mesh model for 3D rendering", editor::style::icon::Mesh)

    //--------------------------------------------------------------------------------------
    bool MeshComponent::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerProperty(MeshComponent, m_displayBones, "Display Bones");
        registerPropertyResource(MeshComponent, m_meshResource, "Mesh");
        registerPropertyObjectEx(MeshComponent, m_meshMaterials, "Materials", IProperty::Flags::Flatten);
        registerProperty(MeshComponent, m_batchMask, "Batch Mask");
        
        return true;
    }

    //--------------------------------------------------------------------------------------
    MeshComponent::MeshComponent(const core::string & _name, IObject * _parent) :
        core::Component(_name, _parent),
        m_meshResource(_name, this),
        m_meshMaterials(_name, this)
    {
        m_meshInstance = (IMeshInstance *)CreateFactoryObject(MeshInstance, _name, this);
    }

    //--------------------------------------------------------------------------------------
    MeshComponent::~MeshComponent()
    {
        // MeshComponent may have no PickingID if the mesh was not found
        auto * picking = Engine::get()->GetRenderer()->GetPicking();
        PickingID id = m_meshInstance->GetPickingID();
        if (id)
            picking->ReleasePickingID(id);

        getGameObject()->removeGraphicInstance(m_meshInstance);
        m_registered = false;
        Engine::get()->GetRenderer()->ReleaseAsync(m_meshInstance);

        VG_SAFE_RELEASE(m_updateSkeletonJob);
        VG_SAFE_RELEASE(m_drawSkeletonJob);
    }

    //--------------------------------------------------------------------------------------
    void MeshComponent::Update(float _dt)
    {
        // TODO: could be done only when GameObject's matrix changes?
        GameObject * go = getGameObject();

        if (nullptr != m_meshInstance)
        {
            m_meshInstance->setGlobalMatrix(go->getGlobalMatrix());
            m_meshInstance->setColor(go->getColor());

            if (asBool(ComponentFlags::NoCulling & getComponentFlags()))
                m_meshInstance->setRuntimeFlags(IInstance::RuntimeFlags::NoCulling, true);
            else
                m_meshInstance->setRuntimeFlags(IInstance::RuntimeFlags::NoCulling, false);
        }

        if (m_meshInstance->IsSkinned())
        {
            if (EngineOptions::get()->useAnimationJobs())
            {
                if (nullptr == m_updateSkeletonJob)
                    m_updateSkeletonJob = new AnimationJob(this, m_displayBones);

                const auto animSync = Engine::get()->getJobSync(JobSync::Animation);

                core::Scheduler * jobScheduler = (core::Scheduler *)Kernel::getScheduler();
                jobScheduler->Start(m_updateSkeletonJob, animSync);

                #if 1
                if (m_displayBones)
                {
                    const auto debugDrawSync = Engine::get()->GetRenderer()->GetJobSync(IRenderer::JobSync::DebugDraw);

                    if (nullptr == m_drawSkeletonJob)
                        m_drawSkeletonJob = new DrawSkeletonJob(this);
                    jobScheduler->StartAfter(animSync, m_drawSkeletonJob, debugDrawSync);
                }
                #endif
            }
            else
            {
                VG_PROFILE_CPU("Animation");

                m_meshInstance->UpdateSkeleton();

                if (m_displayBones)
                    m_meshInstance->DrawSkeleton(getGameObject()->GetWorld());
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void MeshComponent::SetComponentFlags(ComponentFlags _flags, bool _enabled)
    {
        super::SetComponentFlags(_flags, _enabled);

        if (m_meshInstance)
            m_meshInstance->SetInstanceFlags(InstanceFlags::Enabled, asBool(ComponentFlags::Enabled & GetComponentFlags()));
    }

    //--------------------------------------------------------------------------------------
    void MeshComponent::OnPropertyChanged(IObject * _object, const IProperty & _prop, bool _notifyParent)
    {
        if (!strcmp(_prop.getName(), "m_batchMask"))
        {
            if (nullptr != m_meshInstance)
                m_meshInstance->SetBatchMask(m_batchMask);
        }
        else if (!strcmp(_prop.getName(), "m_shader"))
        {
            MaterialResourceData * matResData = dynamic_cast<MaterialResourceData *>(_object->getParent());
            VG_ASSERT(matResData);
            MaterialResource * matRes = dynamic_cast<MaterialResource *>(matResData->getParent());
            VG_ASSERT(matRes);
            const auto & matResources = m_meshMaterials.getMaterialResources();
            for (uint i = 0; i < matResources.size(); ++i)
            {
                if (&matResources[i] == matRes)
                {
                    VG_INFO("[MeshComponent] Material %u shader changed", i);
                    MaterialResourceData * matResData = dynamic_cast<MaterialResourceData *>(matRes->getObject());
                    m_meshInstance->SetMaterial(i, matResData ? matResData->m_materialModel : nullptr);
                    break;
                }
            }
        }
        else if (!strcmp(_prop.getName(), "m_surfaceType"))
        {
            MaterialResourceData * matResData = dynamic_cast<MaterialResourceData *>(_object->getParent());
            VG_ASSERT(matResData);
            MaterialResource * matRes = dynamic_cast<MaterialResource *>(matResData->getParent());
            VG_ASSERT(matRes);
            const auto & matResources = m_meshMaterials.getMaterialResources();
            for (uint i = 0; i < matResources.size(); ++i)
            {
                if (&matResources[i] == matRes)
                {
                    m_meshInstance->OnMaterialChanged(i);
                    break;
                }
            }
        }

        super::OnPropertyChanged(_object, _prop, _notifyParent);
    }

    //--------------------------------------------------------------------------------------
    void MeshComponent::onResourceLoaded(IResource * _resource)
    {
        if (_resource == &m_meshResource)
        {
            // Mesh loaded
            IMeshModel * meshModel = m_meshResource.getMeshModel();
            m_meshInstance->SetModel(Lod::Lod0, meshModel);
            m_batchMask.setBitCount(meshModel->GetBatchCount(), true);
            m_meshInstance->SetBatchMask(m_batchMask);

            if (false == m_registered)
            {
                auto * picking = Engine::get()->GetRenderer()->GetPicking();
                PickingID id = m_meshInstance->GetPickingID();
                if (!id)
                {
                    id = picking->CreatePickingID(this);
                    m_meshInstance->SetPickingID(id);
                }
                
                getGameObject()->addGraphicInstance(m_meshInstance);
                m_meshInstance->setName(getGameObject()->getName().c_str());
                m_registered = true;
            }
        }
        else if (auto matRes = dynamic_cast<MaterialResource *>(_resource))
        {
            // Material resource loaded
            const auto & matResources = m_meshMaterials.getMaterialResources();
            for (uint i = 0; i < matResources.size(); ++i)
            {
                if (&matResources[i] == matRes)
                {
                    //VG_INFO("[MeshComponent] Material %u loaded", i);
                    MaterialResourceData * matResData = dynamic_cast<MaterialResourceData *>(matRes->getObject());
                    m_meshInstance->SetMaterial(i, matResData ? matResData->m_materialModel : nullptr);
                    break;
                }
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void MeshComponent::onResourceUnloaded(core::IResource * _resource)
    {
        if (_resource == &m_meshResource)
        {
            // Mesh unloaded
            m_meshInstance->SetModel(Lod::Lod0, nullptr);
            //m_batchMask.setBitCount(0);

            if (m_registered)
            {
                auto * picking = Engine::get()->GetRenderer()->GetPicking();
                picking->ReleasePickingID(m_meshInstance->GetPickingID());
                m_meshInstance->ClearPickingID();
                getGameObject()->removeGraphicInstance(m_meshInstance);
                m_registered = false;
            }
        }
        else if (auto matRes = dynamic_cast<MaterialResource *>(_resource))
        {
            // Material resource loaded
            const auto & matResources = m_meshMaterials.getMaterialResources();
            for (uint i = 0; i < matResources.size(); ++i)
            {
                if (&matResources[i] == matRes)
                {
                    //VG_INFO("[MeshComponent] Material %u unloaded", i);
                    m_meshInstance->SetMaterial(i, nullptr);
                    break;
                }
            }
        }
    }
}
