#include "engine/Precomp.h"
#include "MeshComponent.h"
#include "core/GameObject/GameObject.h"
#include "renderer/IMeshInstance.h"
#include "renderer/IMeshModel.h"
#include "engine/Engine.h"
#include "renderer/IRenderer.h"
#include "renderer/IPicking.h"

#include "MaterialResourceList.hpp"

#if !VG_ENABLE_INLINE
#include "MeshComponent.inl"
#endif

using namespace vg::core;
using namespace vg::renderer;

namespace vg::engine
{
    VG_REGISTER_COMPONENT_CLASS(MeshComponent, "Mesh", "Rendering", "3D model for rendering")

    //--------------------------------------------------------------------------------------
    bool MeshComponent::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerProperty(MeshComponent, m_displayBones, "Display Bones");
        registerPropertyResource(MeshComponent, m_meshResource, "Mesh");
        registerPropertyObject(MeshComponent, m_meshMaterials, "Materials");
        //registerProperty("MeshComponent", "m_meshInstance", (IObject**)offsetof(MeshComponent, m_meshInstance), "Instance", IProperty::Flags::Hidden);

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
    }

    //--------------------------------------------------------------------------------------
    void MeshComponent::Update(float _dt)
    {
        // TODO: could be done only when GameObject's matrix changes?
        GameObject * go = getGameObject();

        if (nullptr != m_meshInstance)
        {
            m_meshInstance->setWorldMatrix(go->getWorldMatrix());
            m_meshInstance->setColor(go->getColor());
        }

        if (m_meshInstance->IsSkinned())
        {
            m_meshInstance->UpdateSkeleton();

            if (m_displayBones)
                m_meshInstance->ShowSkeleton();
        }
    }

    //--------------------------------------------------------------------------------------
    void MeshComponent::OnPropertyChanged(IObject * _object, const IProperty & _prop, bool _notifyParent)
    {
        if (!strcmp(_prop.getName(), "m_shader"))
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
