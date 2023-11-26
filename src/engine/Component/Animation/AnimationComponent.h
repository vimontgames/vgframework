#pragma once

#include "engine/IAnimationComponent.h"
#include "AnimationResourceList.h"

namespace vg::engine
{
    class AnimationComponent : public IAnimationComponent
    {
    public:
        VG_CLASS_DECL(AnimationComponent, IAnimationComponent);

        AnimationComponent(const core::string & _name, IObject * _parent);
        ~AnimationComponent();

        void                    Update              (double _dt) override;

        void                    onPropertyChanged   (IObject * _object, const core::IProperty & _prop) override;

        void                    onResourceLoaded    (core::IResource * _resource) override;
        void                    onResourceUnloaded  (core::IResource * _resource) override;

    private:
        AnimationResourceList   m_animations;
    };
}