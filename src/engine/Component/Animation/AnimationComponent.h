#pragma once

#include "engine/IAnimationComponent.h"
#include "AnimationResourceList.h"
#include "renderer/IMeshInstance.h" // for renderer::MaxAnimationLayerCount (TODO: separate Animation_consts.h ?)

namespace vg::engine
{
    class MeshComponent;

    class AnimationComponent final : public IAnimationComponent
    {
    public:
        VG_CLASS_DECL(AnimationComponent, IAnimationComponent);

        AnimationComponent(const core::string & _name, IObject * _parent);
        ~AnimationComponent();

        void                    Update                      (const Context & _context) final override;

        void                    OnEnable                    () final override;
        void                    OnDisable                   () final override;

        void                    OnResourceLoaded            (core::IResource * _resource) final override;
        void                    OnResourceUnloaded          (core::IResource * _resource) final override;

        IAnimationResource *    GetAnimation                (core::uint _index) const final override;
        core::uint              GetAnimationIndex           (const core::string & _name) const final override;
        core::uint              GetAnimationCount           () const final override;

        bool                    PlayAnimation               (core::uint _index, bool _loop) final override;
        bool                    StopAnimation               (core::uint _index) final override;

        void                    bindAnimations              ();

    private:
        MeshComponent *         getMeshComponent            () const;
        void                    resetCurrentLayerAnimations ();

    private:
        core::uint              m_currentLayerAnimationIndex[renderer::MaxAnimationLayerCount];
        AnimationResourceList   m_animations;
    };
}