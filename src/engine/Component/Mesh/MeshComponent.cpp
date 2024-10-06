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
#include "engine/Job/Animation/DrawSkeletonJob.h"
#include "engine/Component/Animation/AnimationComponent.h"

#include "MaterialResourceList.hpp"

#if !VG_ENABLE_INLINE
#include "MeshComponent.inl"
#endif

using namespace vg::core;
using namespace vg::renderer;

namespace vg::engine
{
    VG_REGISTER_COMPONENT_CLASS(MeshComponent, "Mesh", "Renderer", "Mesh model for 3D rendering", editor::style::icon::Mesh, 1)

    //--------------------------------------------------------------------------------------
    bool MeshComponent::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyResource(MeshComponent, m_meshResource, "Mesh");
        registerPropertyObjectPtrEx(MeshComponent, m_meshInstance, "MeshInstance", PropertyFlags::NotSaved | PropertyFlags::Flatten);
        registerPropertyObjectEx(MeshComponent, m_meshMaterials, "Materials", PropertyFlags::Flatten);
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
    void MeshComponent::Update(const Context & _context)
    {
        if (nullptr != m_meshInstance)
        {
            m_meshInstance->setGlobalMatrix(_context.m_gameObject->getGlobalMatrix());
            m_meshInstance->setColor(_context.m_gameObject->getColor());
            m_meshInstance->setObjectFlags(ObjectFlags::NoCulling, asBool(ComponentFlags::NoCulling & getComponentFlags()));
        }

        if (m_meshInstance->IsSkinned())
        {
            const bool displayBones = isSkeletonVisible();

            if (EngineOptions::get()->useAnimationJobs())
            {
                if (nullptr == m_updateSkeletonJob)
                    m_updateSkeletonJob = new AnimationJob(this, displayBones);

                const auto animSync = Engine::get()->getJobSync(EngineJobType::Animation);

                core::Scheduler * jobScheduler = (core::Scheduler *)Kernel::getScheduler();
                jobScheduler->Start(m_updateSkeletonJob, animSync);

                if (isSkeletonVisible())
                {
                    const auto debugDrawSync = Engine::get()->GetRenderer()->GetJobSync(RendererJobType::DebugDraw);

                    if (nullptr == m_drawSkeletonJob)
                        m_drawSkeletonJob = new DrawSkeletonJob(this);
                    jobScheduler->StartAfter(animSync, m_drawSkeletonJob, debugDrawSync);
                }
            }
            else
            {
                VG_PROFILE_CPU("Animation");

                m_meshInstance->UpdateSkeleton();

                if (displayBones)
                    m_meshInstance->DrawSkeleton(_context.m_world);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    bool MeshComponent::isSkeletonVisible() const
    {
        return EngineOptions::get()->isShowSkeleton();
    }

    //--------------------------------------------------------------------------------------
    void MeshComponent::EnableComponentFlags(ComponentFlags _flags, bool _enabled)
    {
        super::EnableComponentFlags(_flags, _enabled);

        if (m_meshInstance)
            m_meshInstance->SetInstanceFlags(InstanceFlags::Enabled, asBool(ComponentFlags::Enabled & GetComponentFlags()));
    }

    //--------------------------------------------------------------------------------------
    const core::string MeshComponent::GetSubObjectName(core::uint _subObjectIndex) const
    {
        if (nullptr != m_meshInstance)
            return m_meshInstance->GetBatchName(_subObjectIndex);

        return "";
    };

    //--------------------------------------------------------------------------------------
    void MeshComponent::OnPropertyChanged(IObject * _object, const IProperty & _prop, bool _notifyParent)
    {
        if (!strcmp(_prop.GetName(), "m_batchMask"))
        {
            if (nullptr != m_meshInstance)
                m_meshInstance->SetBatchMask(m_batchMask);
        }
        else if (!strcmp(_prop.GetName(), "m_shader"))
        {
            MaterialResourceData * matResData = dynamic_cast<MaterialResourceData *>(_object->GetParent());
            VG_ASSERT(matResData);
            MaterialResource * matRes = dynamic_cast<MaterialResource *>(matResData->GetParent());
            VG_ASSERT(matRes);
            const auto & matResources = m_meshMaterials.getResources();
            for (uint i = 0; i < matResources.size(); ++i)
            {
                if (&matResources[i] == matRes)
                {
                    VG_INFO("[MeshComponent] Material %u shader changed", i);
                    MaterialResourceData * matResData = dynamic_cast<MaterialResourceData *>(matRes->GetObject());
                    m_meshInstance->SetMaterial(i, matResData ? matResData->m_materialModel : nullptr);
                    break;
                }
            }
        }
        else if (!strcmp(_prop.GetName(), "m_surfaceType") || !strcmp(_prop.GetName(), "m_cullMode"))
        {
            MaterialResourceData * matResData = dynamic_cast<MaterialResourceData *>(_object->GetParent());
            VG_ASSERT(matResData);
            MaterialResource * matRes = dynamic_cast<MaterialResource *>(matResData->GetParent());
            VG_ASSERT(matRes);
            const auto & matResources = m_meshMaterials.getResources();
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
    void MeshComponent::OnResourceLoaded(IResource * _resource)
    {
        if (_resource == &m_meshResource)
        {
            // Mesh loaded
            IMeshModel * meshModel = m_meshResource.getMeshModel();
            m_meshInstance->SetModel(Lod::Lod0, meshModel);
            m_batchMask.setBitCount(meshModel->GetBatchCount(), true);
            m_batchMask.setNames(meshModel->GetBatchNames());
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
                m_meshInstance->SetName(getGameObject()->GetName().c_str());
                m_registered = true;
            }

            // In case Animations were loaded before Mesh we need to rebind them
            if (auto * animComponent = GetGameObject()->GetComponentT<AnimationComponent>())
                animComponent->bindAnimations();

        }
        else if (auto matRes = dynamic_cast<MaterialResource *>(_resource))
        {
            // Material resource loaded
            const auto & matResources = m_meshMaterials.getResources();
            for (uint i = 0; i < matResources.size(); ++i)
            {
                if (&matResources[i] == matRes)
                {
                    //VG_INFO("[MeshComponent] Material %u loaded", i);
                    MaterialResourceData * matResData = dynamic_cast<MaterialResourceData *>(matRes->GetObject());
                    m_meshInstance->SetMaterial(i, matResData ? matResData->m_materialModel : nullptr);
                    break;
                }
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void MeshComponent::OnResourceUnloaded(core::IResource * _resource)
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
            const auto & matResources = m_meshMaterials.getResources();
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

    //--------------------------------------------------------------------------------------
    bool MeshComponent::TryGetAABB(core::AABB & _aabb) const
    {
        if (m_meshInstance)
        {
            if (m_meshInstance->TryGetAABB(_aabb))
                return true;
        }

        return false;
    }
}
