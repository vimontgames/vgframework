#include "engine/Precomp.h"
#include "AnimationComponent.h"
#include "AnimationResourceList.hpp"
#include "core/GameObject/GameObject.h"
#include "engine/Component/Mesh/MeshComponent.h"
#include "renderer/IMeshInstance.h"
#include "renderer/IAnimation.h"
#include "editor/Editor_Consts.h"
#include "core/Math/Math.h"
#include "core/IWorld.h"

using namespace vg::core;
using namespace vg::renderer;

namespace vg::engine
{
    VG_REGISTER_COMPONENT_CLASS(AnimationComponent, "Animation", "Animation", "List of animations to use with a skinned Mesh component", editor::style::icon::Animation, 0);

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
    void AnimationComponent::Update(const Context & _context)
    {
        auto & animResources = m_animations.getResources();

        if (_context.m_world->IsPlaying() && !_context.m_world->IsPaused()) // TODO: check if running from prefab world using context instead? Or include play/stop/paused state in context maybe?
        {
            if (-1 != m_primaryIndex || -1 != m_secondaryIndex)
            {
                float amount = _context.m_dt * 16.0f;

                //if (currentWeight < 1.0f)
                {
                    for (uint i = 0; i < animResources.size(); ++i)
                    {
                        auto & anim = animResources[i];

                        if ((i == m_primaryIndex || i == m_secondaryIndex) && anim.isPlaying())
                        {
                            float weight = anim.getWeight();
                            weight = saturate(weight + amount);
                            anim.setWeight(weight);
                        }
                        else
                        {
                            float weight = anim.getWeight();
                            weight = saturate(weight - amount);
                            anim.setWeight(weight);
                        }
                    }
                }

                float sumAdd = 0.0f;
                float sumBlend = 0.0f;

                for (uint i = 0; i < animResources.size(); ++i)
                {
                    auto & anim = animResources[i];
                    const float weight = anim.getWeight();
                    if (anim.isAdditive())
                        sumAdd += weight;
                    else
                        sumBlend += weight;
                }
                //VG_ASSERT(sumBlend > 0.0f);

                for (uint i = 0; i < animResources.size(); ++i)
                {
                    auto & anim = animResources[i];
                    const float weight = anim.getWeight();

                    if (!anim.isAdditive())
                        anim.setWeight(sumBlend > 0 ? weight / sumBlend : 0);
                }
            }
        }

        for (uint i = 0; i < animResources.size(); ++i)
        {
            AnimationResource & animRes = animResources[i];

            if (animRes.isPlaying())
            {
                if (IAnimation * anim = (IAnimation *)animRes.GetObject())
                {
                    const float speed = animRes.getSpeed();
                    const float duration = anim->GetLength();
                    const float framerate = anim->GetFramerate();
                    
                    float t = animRes.getTime();
                    animRes.setTime(t + _context.m_dt * animRes.getSpeed());
                }
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void AnimationComponent::OnEnable()
    {
        super::OnEnable();
        m_animations.OnEnable();
    }

    //--------------------------------------------------------------------------------------
    void AnimationComponent::OnDisable()
    {
        m_animations.OnDisable();
        super::OnDisable();
    }

    //--------------------------------------------------------------------------------------
    MeshComponent * AnimationComponent::getMeshComponent() const
    {
        MeshComponent * meshComponent = GetGameObject()->GetComponentT<MeshComponent>();
        return meshComponent;
    }

    //--------------------------------------------------------------------------------------
    void AnimationComponent::bindAnimations()
    {
        MeshComponent * meshComponent = getMeshComponent();
        if (meshComponent)
        {
            auto & animResources = m_animations.getResources();
            for (uint i = 0; i < animResources.size(); ++i)
            {
                AnimationResource & animRes = animResources[i];
                if (renderer::ISkeletalAnimation * anim = (renderer::ISkeletalAnimation *)animRes.GetObject())
                    meshComponent->getMeshInstance()->AddAnimation(anim);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void AnimationComponent::OnResourceLoaded(IResource * _resource)
    {
        MeshComponent * meshComponent = getMeshComponent();
        if (meshComponent)
        {
            renderer::ISkeletalAnimation * anim = (renderer::ISkeletalAnimation *)_resource->GetObject();
            meshComponent->getMeshInstance()->AddAnimation(anim);
        }
    }

    //--------------------------------------------------------------------------------------
    void AnimationComponent::OnResourceUnloaded(core::IResource * _resource)
    {
        MeshComponent * meshComponents = GetGameObject()->GetComponentT<MeshComponent>();
        if (meshComponents)
        {
            renderer::ISkeletalAnimation * anim = (renderer::ISkeletalAnimation *)_resource->GetObject();
            meshComponents->getMeshInstance()->RemoveAnimation(anim);
        }
    }

    //--------------------------------------------------------------------------------------
    IAnimationResource * AnimationComponent::GetAnimation(core::uint _index) const
    {
        auto & anims = m_animations.getResources();
        if (_index < GetAnimationCount())
            return (IAnimationResource *) &anims[_index];
        else
            return nullptr;
    }

    //--------------------------------------------------------------------------------------
    core::uint AnimationComponent::GetAnimationIndex(const core::string & _name) const
    {
        auto & anims = m_animations.getResources();
        for (uint i = 0; i < anims.size(); ++i)
        {
            auto & anim = anims[i];
            if (anim.GetName() == _name)
                return i;
        }
        VG_WARNING("[Animation] GameObject \"%s\" not find Animation \"%s\"", GetGameObject()->GetName().c_str(), _name.c_str());
        return -1;
    }

    //--------------------------------------------------------------------------------------
    core::uint AnimationComponent::GetAnimationCount() const
    {
        return (uint)m_animations.getResources().size();
    }

    //--------------------------------------------------------------------------------------
    bool AnimationComponent::PlayAnimation(core::uint _index, bool _loop)
    {
        if (-1 != _index)
        {
            auto & anims = m_animations.getResources();
            auto & anim = anims[_index];

            if (anim.isAdditive())
            {
                //if (_index != m_secondaryIndex)
                {
                    anim.setTime(0.0f);
                    anim.setPlay(true);
                    anim.setLoop(_loop);

                    m_secondaryIndex = _index;

                    return true;
                }
            }
            else
            {
                if (_index != m_primaryIndex)
                {
                    if (!anim.isPlaying())
                    {
                        anim.setTime(0.0f);
                        anim.setPlay(true);
                        anim.setLoop(_loop);
                    }

                    m_primaryIndex = _index;

                    return true;
                }
            }            
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool AnimationComponent::StopAnimation(core::uint _index)
    {
        if (-1 != _index)
        {
            auto & anims = m_animations.getResources();
            auto & anim = anims[_index];

            if (anim.isAdditive())
            {
                if (_index == m_secondaryIndex)
                {
                    anim.setPlay(false);
                    return true;
                }
            }
            else
            {
                if (_index == m_primaryIndex)
                {
                    anim.setPlay(false);
                    return true;
                }
            }
        }

        return false;
    }
}
