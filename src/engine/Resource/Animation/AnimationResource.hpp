#include "AnimationResource.h"
#include "renderer/IAnimation.h"
#include "core/Math/Math.h"
#include "core/GameObject/GameObject.h"
#include "engine/Component/Animation/AnimationComponent.h"
#include "engine/Component/Mesh/MeshComponent.h"
#include "renderer/IMeshInstance.h"

using namespace vg::core;
using namespace vg::renderer;

namespace vg::engine
{
    VG_REGISTER_RESOURCE_CLASS(AnimationResource, "Animation");

    //--------------------------------------------------------------------------------------
    void * ResizeAnimationResourceVector(IObject * _parent, uint _offset, uint _count, uint & _elementSize)
    {
        auto vec = (core::vector<AnimationResource> *)(uint_ptr(_parent) + _offset);
        vec->clear();
        vec->resize(_count);
        _elementSize = sizeof(AnimationResource);
        return vec->data();
    }

    //--------------------------------------------------------------------------------------
    bool AnimationResource::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyCallbackEx(AnimationResource, playAnim, "Play", IProperty::Flags::SingleLine);
        registerPropertyCallbackEx(AnimationResource, stopAnim, "Stop", IProperty::Flags::SingleLine);

        registerProperty(AnimationResource, m_play, "Play");
        registerPropertyEx(AnimationResource, m_loop, "Loop", IProperty::Flags::SingleLine);

        setPropertyFlag(AnimationResource, m_name, IProperty::Flags::NotVisible, false);

        registerPropertyEx(AnimationResource, m_time, "Time", IProperty::Flags::NotSaved);

        registerPropertyEx(AnimationResource, m_weight, "Weight", IProperty::Flags::NotSaved);
        setPropertyRange(AnimationResource, m_weight, float2(0.0f, 1.0f));

        registerProperty(AnimationResource, m_speed, "Speed");
        setPropertyRange(AnimationResource, m_speed, float2(0.0f, 10.0f));

        registerResizeVectorFunc(AnimationResource, ResizeAnimationResourceVector);

        return true;
    }

    //--------------------------------------------------------------------------------------
    AnimationResource::AnimationResource(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    AnimationResource::~AnimationResource()
    {
        ResourceManager::get()->unloadResource(this, GetResourcePath());
    }

    //--------------------------------------------------------------------------------------
    const core::vector<core::string> AnimationResource::getExtensions() const
    {
        vector<string> ext;
        ext.push_back(".fbx");
        return ext;
    }

    //--------------------------------------------------------------------------------------
    void AnimationResource::onResourcePathChanged(const string & _oldPath, const string & _newPath)
    {
        if (_oldPath != _newPath)
            ResourceManager::get()->loadResourceAsync(this, _oldPath, _newPath);
    }

    //--------------------------------------------------------------------------------------
    bool AnimationResource::cook(const string & _file) const
    {
        return Engine::get()->GetRenderer()->cookAnimation(_file);
    }

    //--------------------------------------------------------------------------------------
    core::IObject * AnimationResource::load(const string & _file)
    {
        IAnimation * anim = Engine::get()->GetRenderer()->loadAnimation(_file);
        return anim;
    }

    //--------------------------------------------------------------------------------------
    bool AnimationResource::playAnim(IObject * _object)
    {
        return ((AnimationResource *)_object)->PlayOnce();
    }

    //--------------------------------------------------------------------------------------
    bool AnimationResource::stopAnim(IObject * _object)
    {
        return ((AnimationResource *)_object)->Stop();
    }

    //--------------------------------------------------------------------------------------
    bool AnimationResource::PlayOnce()
    {
        if (!m_play)
        {
            setTime(0.0f);
            setWeight(1.0f);
            m_play = true;
            m_loop = false;
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool AnimationResource::IsPlaying() const
    {
        if (IAnimation * anim = (IAnimation *)getObject())
            return m_play;
        else
            return false;
    }

    //--------------------------------------------------------------------------------------
    bool AnimationResource::IsFinished() const
    {
        if (IAnimation * anim = (IAnimation *)getObject())
            return !m_loop && m_time >= anim->GetLength();
        else
            return true;
    }

    //--------------------------------------------------------------------------------------
    bool AnimationResource::PlayLoop()
    {
        if (!getObject())
            return false;

        if (!m_play)
        {
            setTime(0.0f);
            setWeight(1.0f);
            m_play = true;
            m_loop = true;
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool AnimationResource::Stop()
    {
        if (m_play)
        {
            setTime(0.0f);
            setWeight(0.0f);
            m_play = false;
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    float AnimationResource::GetWeight() const
    {
        return m_weight;
    }

    //--------------------------------------------------------------------------------------
    void AnimationResource::OnEnable()
    {

    }

    //--------------------------------------------------------------------------------------
    void AnimationResource::OnDisable()
    {
        reset();
    }

    //--------------------------------------------------------------------------------------
    void AnimationResource::OnStop()
    {
        super::OnStop();
        setTime(m_time);
        setWeight(m_weight);
    }

    //--------------------------------------------------------------------------------------
    void AnimationResource::reset()
    {
        setTime(0.0f);
        setWeight(0.0f);
    }

    //--------------------------------------------------------------------------------------
    void AnimationResource::OnPropertyChanged(core::IObject * _object, const core::IProperty & _prop, bool _notifyParent)
    {
        if (!strcmp(_prop.getName(), "m_time"))
            setTime(m_time);
        else if (!strcmp(_prop.getName(), "m_weight"))
            setWeight(m_weight);
    }

    //--------------------------------------------------------------------------------------
    IMeshInstance * AnimationResource::getMeshInstance()
    {
        AnimationComponent * animComponent = dynamic_cast<AnimationComponent *>(getParent()->getParent());
        VG_ASSERT(animComponent);
        IGameObject * go = animComponent->GetGameObject();
        VG_ASSERT(go);
        MeshComponent * meshComponent = go->GetComponentT<MeshComponent>();
        VG_ASSERT(meshComponent);
        IMeshInstance * meshInstance = meshComponent->getMeshInstance();
        VG_ASSERT(meshInstance);
        return meshInstance;
    }

    //--------------------------------------------------------------------------------------
    void AnimationResource::setTime(float _time)
    {
        if (IAnimation * anim = (IAnimation *)getObject())
        {
            const float animLength = anim->GetLength();

            if (m_loop)
                m_time = fmod(_time, animLength);
            else
                m_time = _time;

            if (IMeshInstance * meshInstance = getMeshInstance())
                meshInstance->SetAnimationTime((ISkeletalAnimation*)anim, m_time);
        }
    }

    //--------------------------------------------------------------------------------------
    void AnimationResource::setWeight(float _weight)
    {
        m_weight = _weight;

        if (m_weight == 0.0f)
            m_play = false;

        if (IAnimation * anim = (IAnimation *)getObject())
        {
            if (IMeshInstance * meshInstance = getMeshInstance())
                meshInstance->SetAnimationWeight((ISkeletalAnimation *)anim, m_weight);
        }
    }    
}