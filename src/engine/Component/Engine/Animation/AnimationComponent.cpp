#include "engine/Precomp.h"
#include "AnimationComponent.h"
#include "AnimationResourceList.hpp"
#include "core/GameObject/GameObject.h"
#include "engine/Component/Renderer/Instance/Mesh/MeshComponent.h"
#include "engine/Engine.h"
#include "engine/EngineOptions.h"
#include "renderer/IMeshInstance.h"
#include "renderer/IAnimation.h"
#include "renderer/ISkeletalAnimation.h"
#include "editor/Editor_Consts.h"
#include "core/Math/Math.h"
#include "core/IWorld.h"

using namespace vg::core;
using namespace vg::renderer;

namespace vg::engine
{
    VG_REGISTER_COMPONENT_CLASS(AnimationComponent, "Animation", "Engine", "List of animations to use with a skinned Mesh component", editor::style::icon::Animation, getPriority(ComponentGroup::Render, ComponentPriority::Early, ComponentMultithreadType::Job));

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
        resetCurrentLayerAnimations();
    }

    //--------------------------------------------------------------------------------------
    AnimationComponent::~AnimationComponent()
    {
        
    }

    //--------------------------------------------------------------------------------------
    void AnimationComponent::resetCurrentLayerAnimations()
    {
        for (uint i = 0; i < MaxAnimationLayerCount; ++i)
            m_currentLayerAnimationIndex[i] = -1;
    }

    //--------------------------------------------------------------------------------------
    void AnimationComponent::Update(const Context & _context)
    {
        auto & animResources = m_animations.getResources();

        if (_context.m_playing && !_context.m_paused)
        {
            float amount = _context.m_dt * 4.0f;    // Animation blending speed should be exposed (in 'PlayAnimation'?)

            float sum[MaxAnimationLayerCount];

            for (uint layerIndex = 0; layerIndex < MaxAnimationLayerCount; ++layerIndex)
            {
                const auto currentLayerAnimIndex = m_currentLayerAnimationIndex[layerIndex];
                sum[layerIndex] = 0.0f;

                // Update animation time
                for (uint i = 0; i < animResources.size(); ++i)
                {
                    auto & anim = animResources[i];
                    float weight = anim.getWeight();
                    const auto layer = anim.getLayer();

                    if (layerIndex == layer)
                    {
                        if (currentLayerAnimIndex == i && anim.isPlaying())
                        {
                            // Increase weight if animation is playing
                            weight = saturate(weight + amount);
                            anim.setWeight(weight);
                        }
                        else
                        {
                            if (weight > 0.0f)
                            {
                                // Decrease weight if animation if no more playing
                                weight = saturate(weight - amount);
                                anim.setWeight(weight);
                            }
                            else
                            {
                                anim.setTime(0.0f);
                            }
                        }

                        // Sum weights per layer
                        sum[layerIndex] += weight;
                    }
                }
            }

            float invSum[MaxAnimationLayerCount];
            for (uint layerIndex = 0; layerIndex < MaxAnimationLayerCount; ++layerIndex)
                invSum[layerIndex] = sum[layerIndex] != 0.0f ? 1.0f / sum[layerIndex] : 0.0f;

            // Normalize layer weights
            for (uint layerIndex = 0; layerIndex < MaxAnimationLayerCount; ++layerIndex)
            {               
                if (invSum[layerIndex] != 0.0f)
                {
                    for (uint i = 0; i < animResources.size(); ++i)
                    {
                        auto & anim = animResources[i];
                        const auto layer = anim.getLayer();

                        if (layerIndex == layer && layerIndex == 0)
                        {
                            const float weight = anim.getWeight();
                            anim.setWeight(weight * invSum[layerIndex]);
                        }
                    }
                }
            }
        }

        auto * engine = Engine::get();
        const float dt = engine->IsPlaying() ? _context.m_dt : engine->getTime().m_realDeltaTime;

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
                    animRes.setTime(t + dt * animRes.getSpeed());
                }
            }
        }

        // When MeshComponent are updated in jobs, it's better to start skinning job here:
        // - They start earlier
        // - The scheduling is more balanced (no more mixing skinned and non-skinned mesh)
        const auto * options = EngineOptions::get();
        if (options->useAnimationJobs())
        {
            if (options->useComponentUpdateJobs())
            {
                if (auto * meshComp = GetGameObject()->GetComponentT<MeshComponent>())
                    meshComp->computeAnimation(_context.m_world);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void AnimationComponent::OnEnable()
    {
        super::OnEnable();
        resetCurrentLayerAnimations();
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
            VG_PROFILE_CPU("bindAnimations");
            auto & animResources = m_animations.getResources();
            for (uint i = 0; i < animResources.size(); ++i)
            {
                AnimationResource & animRes = animResources[i];
                if (renderer::ISkeletalAnimation * anim = (renderer::ISkeletalAnimation *)animRes.GetObject())
                {
                    auto * meshInstance = meshComponent->getMeshInstance();
                    meshInstance->AddAnimation(anim);
                    meshInstance->SetAnimationLayer(anim, animRes.getLayer());
                    meshInstance->SetAnimationBodyParts(anim, animRes.getBodyParts());
                }
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void AnimationComponent::OnResourceLoaded(IResource * _resource)
    {
        MeshComponent * meshComponent = getMeshComponent();
        if (meshComponent)
        {
            VG_PROFILE_CPU("AnimationComponent");

            AnimationResource * animRes = VG_SAFE_STATIC_CAST(AnimationResource, _resource);
            if (renderer::ISkeletalAnimation * anim = VG_SAFE_STATIC_CAST(renderer::ISkeletalAnimation, animRes->GetObject()))
            {
                auto * meshInstance = meshComponent->getMeshInstance();
                meshInstance->AddAnimation(anim);
                meshInstance->SetAnimationLayer(anim, animRes->getLayer());
                meshInstance->SetAnimationBodyParts(anim, animRes->getBodyParts());
            }
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
        VG_INFO("[Animation] GameObject \"%s\" has no Animation \"%s\"", GetGameObject()->GetName().c_str(), _name.c_str());
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
            const auto layer = anim.getLayer();

            if (_index != m_currentLayerAnimationIndex[layer] || !anim.isPlaying())
            {
                if (!anim.isPlaying())
                {
                    anim.setTime(0.0f);
                    anim.setPlay(true);
                    anim.setLoop(_loop);
                }

                m_currentLayerAnimationIndex[layer] = _index;

                return true;
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
            const auto layer = anim.getLayer();

            if (_index == m_currentLayerAnimationIndex[layer])
            {
                anim.setPlay(false);
                return true;
            }
        }

        return false;
    }
}
