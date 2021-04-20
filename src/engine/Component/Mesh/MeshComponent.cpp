#include "engine/Precomp.h"
#include "MeshComponent.h"
#include "core/Entity/Entity.h"
#include "core/Object/AutoRegisterClass.h"
#include "core/ISector.h"
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
    bool MeshComponent::registerClass(IObjectFactory & _factory)
    {
        if (core::IObjectDescriptor * desc = _factory.registerClassHelper(MeshComponent, "Mesh Component", IObjectDescriptor::Flags::Component))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool MeshComponent::registerProperties(IObjectDescriptor & _desc)
    {
        super::registerProperties(_desc);

        _desc.registerProperty("m_meshResource", (core::IResource**)offsetof(MeshComponent, m_meshResource), "Resource", IPropertyDescriptor::Flags::None);
        _desc.registerProperty("m_meshInstance", (IObject**)offsetof(MeshComponent, m_meshInstance), "Instance", IPropertyDescriptor::Flags::None);

        return true;
    }

    //--------------------------------------------------------------------------------------
    MeshComponent::MeshComponent(const core::string & _name, IObject * _parent) :
        core::Component(_name, _parent),
        m_meshResource(this)
    {
        VG_ASSERT(dynamic_cast<IEntity*>(_parent));
        m_meshInstance = (IMeshInstance*)CreateFactoryObject(MeshInstance, _name, this);
    }

    //--------------------------------------------------------------------------------------
    MeshComponent::~MeshComponent()
    {
        VG_SAFE_RELEASE(m_meshInstance);
    }

    //--------------------------------------------------------------------------------------
    void MeshComponent::update(double _dt)
    {
        m_meshInstance->setWorldMatrix(getEntity()->getWorldMatrix());
    }

    //--------------------------------------------------------------------------------------
    void MeshComponent::onResourceLoaded(IResource * _resource)
    {
        IMeshModel * meshModel = m_meshResource.getMeshModel();
        m_meshInstance->SetModel(Lod::Lod0, meshModel);

        if (false == m_registered)
        {
            IEntity * entity = getEntity();
            ISector * sector = entity->getSector();

            sector->addGraphicInstance(m_meshInstance);

            m_registered = true;
        }
    }
}
