#include "engine/Precomp.h"
#include "MeshComponent.h"
#include "core/GameObject/GameObject.h"
#include "renderer/IMeshInstance.h"
#include "renderer/IMeshModel.h"
#include "engine/Engine.h"
#include "renderer/IRenderer.h"
#include "renderer/IPicking.h"
#include "engine/Resource/Material/MaterialResourceData.h"

#include "MaterialResourceList.hpp"

using namespace vg::core;
using namespace vg::renderer;

namespace vg::engine
{
    VG_AUTO_REGISTER_CLASS(MeshComponent);

    //--------------------------------------------------------------------------------------
    bool MeshComponent::registerClass(IFactory & _factory)
    {
        auto a = [](const vg::core::string & _name, vg::core::IObject * _parent) { return new MeshComponent(_name, _parent); };
        IClassDesc::Func f = [](const vg::core::string & _name, vg::core::IObject * _parent) { return new MeshComponent(_name, _parent); };

        if (core::IClassDesc * desc = _factory.registerClassHelper(MeshComponent, "Mesh Component", IClassDesc::Flags::Component))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool MeshComponent::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        //_desc.registerProperty("MeshComponent", "m_meshResource", (core::IResource**)offsetof(MeshComponent, m_meshResource), "Resource", IProperty::Flags::None);
        _desc.registerPropertyResourceHelper(MeshComponent, m_meshResource, "Resource", IProperty::Flags::Resource);
        _desc.registerPropertyObjectHelper(MeshComponent, m_meshMaterials, "Materials", IProperty::Flags::None);

        //_desc.registerProperty("MeshComponent", "m_meshInstance", (IObject**)offsetof(MeshComponent, m_meshInstance), "Instance", IProperty::Flags::Hidden);

        return true;
    }

    //--------------------------------------------------------------------------------------
    MeshComponent::MeshComponent(const core::string & _name, IObject * _parent) :
        core::Component(_name, _parent),
        m_meshResource(_name, this),
        m_meshMaterials(_name, this)
    {
        VG_ASSERT(dynamic_cast<IGameObject*>(_parent));
        m_meshInstance = (IMeshInstance *)CreateFactoryObject(MeshInstance, _name, this);
    }

    //--------------------------------------------------------------------------------------
    MeshComponent::~MeshComponent()
    {
        auto * picking = Engine::get()->GetRenderer()->GetPicking();
        PickingID id = picking->GetPickingID(this);
        picking->ReleasePickingID(id);

        GameObject * gameObject = getGameObject();
        gameObject->RemoveGraphicInstance(m_meshInstance);
        m_registered = false;
        Engine::get()->GetRenderer()->ReleaseAsync(m_meshInstance);
    }

    //--------------------------------------------------------------------------------------
    void MeshComponent::Update(double _dt)
    {
        // TODO: could be done only when GameObject's matrix changes?
        GameObject * go = getGameObject();

        if (nullptr != m_meshInstance)
        {
            m_meshInstance->setWorldMatrix(go->getWorldMatrix());
            m_meshInstance->setColor(go->getColor());
        }
    }

    //--------------------------------------------------------------------------------------
    void MeshComponent::onPropertyChanged(IObject * _object, const IProperty & _prop)
    {
        // TODO : handle material property changes
        super::onPropertyChanged(_object, _prop);
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
                PickingID id = picking->GetPickingID(this);
                m_meshInstance->setPickingID(id);

                GameObject * gameObject = getGameObject();
                gameObject->AddGraphicInstance(m_meshInstance);

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
                    VG_INFO("[MeshComponent] Material %u needs update", i);
                    break;
                }
            }
        }
        else if (auto texRes = dynamic_cast<TextureResource *>(_resource))
        {
            // Texture loaded
            const auto & matResources = m_meshMaterials.getMaterialResources();
            for (uint i = 0; i < matResources.size(); ++i)
            {
                const auto & matRes = matResources[i];
                const auto matResData = (MaterialResourceData*)matRes.getObject();
                if (nullptr != matResData)
                {
                    const auto textures = matResData->m_textures;
                    for (uint j = 0; j < core::enumCount<renderer::MaterialTextureType>(); ++j)
                    {
                        const auto & tex = textures[j];
                        if (&tex == texRes)
                        {
                            VG_INFO("[MeshComponent] Texture %u from Material %u needs update", j,i);
                            break;
                        }
                    }
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
                picking->ReleasePickingID(m_meshInstance->getPickingID());
                m_meshInstance->resetPickingID();
                GameObject * gameObject = getGameObject();
                gameObject->RemoveGraphicInstance(m_meshInstance);

                m_registered = false;
            }
        }
        else if (auto matRes = dynamic_cast<MaterialResource *>(_resource))
        {
            // Material resource unloaded
        }
        else if (auto texRes = dynamic_cast<TextureResource *>(_resource))
        {
            // Texture unloaded
        }
    }
}
