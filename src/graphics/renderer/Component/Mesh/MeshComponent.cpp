#include "graphics/renderer/Precomp.h"
#include "MeshComponent.h"
#include "core/Entity/Entity.h"
#include "core/Object/AutoRegisterClass.h"

using namespace vg::core;

namespace vg::graphics::renderer
{
    VG_AUTO_REGISTER_CLASS(MeshComponent)

    //--------------------------------------------------------------------------------------
    bool MeshComponent::registerClass(IObjectFactory & _factory)
    {
        if (core::IObjectDescriptor * desc = _factory.registerClassHelper(MeshComponent, "Mesh Component"))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool MeshComponent::registerProperties(IObjectDescriptor & _desc)
    {
        super::registerProperties(_desc);

        _desc.registerProperty("m_model", (IResource**)offsetof(MeshComponent,m_model), "Model", IPropertyDescriptor::Flags::None);

        return true;
    }

    //--------------------------------------------------------------------------------------
    MeshComponent::MeshComponent() :
        core::Component("MeshComponent")
    {

    }

    //--------------------------------------------------------------------------------------
    MeshComponent::~MeshComponent()
    {
        
    }

    //--------------------------------------------------------------------------------------
    void MeshComponent::update(const Entity * _entity, double _dt)
    {
       
    }
}
