#include "engine/Precomp.h"
#include "AnimationComponent.h"
#include "AnimationResourceList.hpp"
#include "core/GameObject/GameObject.h"
#include "engine/Component/Mesh/MeshComponent.h"
#include "renderer/IMeshInstance.h"
#include "renderer/IAnimation.h"

using namespace vg::core;
using namespace vg::renderer;

namespace vg::engine
{
    VG_REGISTER_COMPONENT_CLASS(AnimationComponent, "Animation Component");

    //--------------------------------------------------------------------------------------
    bool AnimationComponent::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyObject(AnimationComponent, m_animations, "Animations");
        
        return true;
    }

    //--------------------------------------------------------------------------------------
    AnimationComponent::AnimationComponent(const core::string & _name, IObject * _parent) :
        super(_name, _parent),
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
        auto & animResources = m_animations.getAnimationResources();
        for (uint i = 0; i < animResources.size(); ++i)
        {
            AnimationResource & animRes = animResources[i];

            if (animRes.isPlaying())
            {
                if (IAnimation * anim = (IAnimation *)animRes.getObject())
                {
                    const float speed = animRes.getSpeed();
                    const float duration = anim->GetLength();
                    const float framerate = anim->GetFramerate();
                    
                    float t = animRes.getTime();
                    animRes.setTime((float)(t + _dt * 0.001f));
                }
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void AnimationComponent::OnPlay()
    {
        super::OnPlay();
        m_animations.OnPlay();
    }

    //--------------------------------------------------------------------------------------
    void AnimationComponent::OnStop()
    {
        m_animations.OnStop();
        super::OnStop();
    }

    //--------------------------------------------------------------------------------------
    void AnimationComponent::OnPropertyChanged(IObject * _object, const IProperty & _prop, bool _notifyParent)
    {
        // [...]
        super::OnPropertyChanged(_object, _prop, _notifyParent);
    }

    //--------------------------------------------------------------------------------------
    MeshComponent * AnimationComponent::getMeshComponent() const
    {
        MeshComponent * meshComponent = GetGameObject()->GetComponentByType<MeshComponent>();
        return meshComponent;
    }

    //--------------------------------------------------------------------------------------
    void AnimationComponent::onResourceLoaded(IResource * _resource)
    {
        MeshComponent * meshComponent = getMeshComponent();
        if (meshComponent)
        {
            renderer::ISkeletalAnimation * anim = (renderer::ISkeletalAnimation *)_resource->getObject();
            meshComponent->getMeshInstance()->AddAnimation(anim);
        }

        super::onResourceLoaded(_resource);
    }

    //--------------------------------------------------------------------------------------
    void AnimationComponent::onResourceUnloaded(core::IResource * _resource)
    {
        MeshComponent * meshComponents = GetGameObject()->GetComponentByType<MeshComponent>();
        if (meshComponents)
        {
            renderer::ISkeletalAnimation * anim = (renderer::ISkeletalAnimation *)_resource->getObject();
            meshComponents->getMeshInstance()->RemoveAnimation(anim);
        }

        super::onResourceUnloaded(_resource);
    }

    //--------------------------------------------------------------------------------------
    IAnimationResource * AnimationComponent::GetAnimation(const core::string & _name)
    {
        auto & anims = m_animations.getAnimationResources();
        for (uint i = 0; i < anims.size(); ++i)
        {
            auto & anim = anims[i];
            if (anim.getName() == _name)
                return &anim;
        }
        VG_WARNING("[Animation] Could not find Animation \"%s\"", _name.c_str());
        return nullptr;
    }
}
