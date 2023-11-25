#include "engine/Precomp.h"
#include "AnimationComponent.h"
#include "AnimationResourceList.hpp"
#include "core/GameObject/GameObject.h"
#include "engine/Component/Mesh/MeshComponent.h"
#include "renderer/IMeshInstance.h"

using namespace vg::core;

namespace vg::engine
{
    VG_AUTO_REGISTER_CLASS(AnimationComponent);

    //--------------------------------------------------------------------------------------
    bool AnimationComponent::registerClass(IFactory & _factory)
    {
        if (core::IClassDesc * desc = _factory.registerClassHelper(AnimationComponent, "Animation Component", IClassDesc::Flags::Component))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool AnimationComponent::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        _desc.registerPropertyObject(AnimationComponent, m_animations, "Animations");
        
        return true;
    }

    //--------------------------------------------------------------------------------------
    AnimationComponent::AnimationComponent(const core::string & _name, IObject * _parent) :
        core::Component(_name, _parent),
        m_animations(_name, this)
    {

    }

    //--------------------------------------------------------------------------------------
    AnimationComponent::~AnimationComponent()
    {
        
    }

    //--------------------------------------------------------------------------------------
    void AnimationComponent::Update(double _dt)
    {
        
    }

    //--------------------------------------------------------------------------------------
    void AnimationComponent::onPropertyChanged(IObject * _object, const IProperty & _prop)
    {
        // [...]
        super::onPropertyChanged(_object, _prop);
    }

    //--------------------------------------------------------------------------------------
    void AnimationComponent::onResourceLoaded(IResource * _resource)
    {
        MeshComponent * meshComponents = getGameObject()->findComponent<MeshComponent>();
        if (meshComponents)
        {
            renderer::ISkeletalAnimation * anim = (renderer::ISkeletalAnimation *)_resource->getObject();
            meshComponents->m_meshInstance->AddAnimation(anim);
        }

        super::onResourceLoaded(_resource);
    }

    //--------------------------------------------------------------------------------------
    void AnimationComponent::onResourceUnloaded(core::IResource * _resource)
    {
        MeshComponent * meshComponents = getGameObject()->findComponent<MeshComponent>();
        if (meshComponents)
        {
            renderer::ISkeletalAnimation * anim = (renderer::ISkeletalAnimation *)_resource->getObject();
            meshComponents->m_meshInstance->RemoveAnimation(anim);
        }

        super::onResourceUnloaded(_resource);
    }
}
