#pragma once

#include "engine/IAnimationComponent.h"
#include "AnimationResourceList.h"

namespace vg::engine
{
    class MeshComponent;

    class AnimationComponent final : public IAnimationComponent
    {
    public:
        VG_CLASS_DECL(AnimationComponent, IAnimationComponent);

        AnimationComponent(const core::string & _name, IObject * _parent);
        ~AnimationComponent();

        void                    Update              (double _dt) final override;

        void                    OnPlay              () final override;
        void                    OnStop              () final override;

        void                    OnPropertyChanged   (IObject * _object, const core::IProperty & _prop, bool _notifyParent) final override;

        void                    onResourceLoaded    (core::IResource * _resource) final override;
        void                    onResourceUnloaded  (core::IResource * _resource) final override;

        IAnimationResource *    GetAnimation        (const core::string & _name) final override;

    private:
        MeshComponent *         getMeshComponent    () const;

    private:
        AnimationResourceList   m_animations;
    };
}