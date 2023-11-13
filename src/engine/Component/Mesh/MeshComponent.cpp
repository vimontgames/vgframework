#include "engine/Precomp.h"
#include "MeshComponent.h"
#include "core/GameObject/GameObject.h"
#include "renderer/IMeshInstance.h"
#include "renderer/IMeshModel.h"
#include "engine/Engine.h"
#include "renderer/IRenderer.h"
#include "renderer/IPicking.h"

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

        _desc.registerProperty("MeshComponent", "m_meshResource", (core::IResource**)offsetof(MeshComponent, m_meshResource), "Resource", IProperty::Flags::None);
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
    void MeshComponent::onResourceLoaded(IResource * _resource)
    {
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

    //--------------------------------------------------------------------------------------
    void MeshComponent::onResourceUnloaded(core::IResource * _resource)
    {
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
}
