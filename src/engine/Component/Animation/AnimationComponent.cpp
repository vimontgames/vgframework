#include "engine/Precomp.h"
#include "AnimationComponent.h"
#include "AnimationResourceList.hpp"

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

        _desc.registerPropertyObjectHelper(AnimationComponent, m_animations, "Animations", IProperty::Flags::None);
        
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
        // [...]
        super::onResourceLoaded(_resource);
    }

    //--------------------------------------------------------------------------------------
    void AnimationComponent::onResourceUnloaded(core::IResource * _resource)
    {
        // [...]
        super::onResourceUnloaded(_resource);
    }
}
