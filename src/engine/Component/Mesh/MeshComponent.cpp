#include "engine/Precomp.h"
#include "MeshComponent.h"
#include "core/GameObject/GameObject.h"
#include "graphics/renderer/IMeshInstance.h"
#include "graphics/renderer/IMeshModel.h"
#include "engine/Engine.h"
#include "graphics/renderer/IRenderer.h"

using namespace vg::core;
using namespace vg::graphics::renderer;

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
        _desc.registerProperty("MeshComponent", "m_meshInstance", (IObject**)offsetof(MeshComponent, m_meshInstance), "Instance", IProperty::Flags::Hidden);

        return true;
    }

    //--------------------------------------------------------------------------------------
    MeshComponent::MeshComponent(const core::string & _name, IObject * _parent) :
        core::Component(_name, _parent),
        m_meshResource(_name, this)
    {
        VG_ASSERT(dynamic_cast<IGameObject*>(_parent));
        m_meshInstance = (IMeshInstance*)CreateFactoryObject(MeshInstance, _name, this);
    }

    //--------------------------------------------------------------------------------------
    MeshComponent::~MeshComponent()
    {
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
        m_meshInstance->setWorldMatrix(go->getWorldMatrix());
        m_meshInstance->setColor(go->getColor());
    }

    //--------------------------------------------------------------------------------------
    void MeshComponent::onResourceLoaded(IResource * _resource)
    {
        IMeshModel * meshModel = m_meshResource.getMeshModel();
        m_meshInstance->SetModel(Lod::Lod0, meshModel);

        if (false == m_registered)
        {
            GameObject * gameObject = getGameObject();
            gameObject->AddGraphicInstance(m_meshInstance);

            m_registered = true;
        }
    }
}
