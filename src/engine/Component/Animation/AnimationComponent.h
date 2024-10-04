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

        void                    Update              (const Context & _context) final override;

        void                    OnEnable            () final override;
        void                    OnDisable           () final override;

        void                    onResourceLoaded    (core::IResource * _resource) final override;
        void                    onResourceUnloaded  (core::IResource * _resource) final override;

        IAnimationResource *    GetAnimation        (core::uint _index) const final override;
        core::uint              GetAnimationIndex   (const core::string & _name) const final override;
        core::uint              GetAnimationCount   () const final override;

        bool                    PlayAnimation       (core::uint _index, bool _loop) final override;
        bool                    StopAnimation       (core::uint _index) final override;

        void                    bindAnimations      ();

    private:
        MeshComponent *         getMeshComponent    () const;

    private:
        core::uint              m_primaryIndex = -1;
        core::uint              m_secondaryIndex = -1;
        AnimationResourceList   m_animations;
    };
}