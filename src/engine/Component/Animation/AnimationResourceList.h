#pragma once

#include "engine/IResourceList.h"
#include "engine/Resource/Animation/AnimationResource.h"

namespace vg::engine
{
    class AnimationResourceList : public IResourceList
    {
    public:
        VG_CLASS_DECL(AnimationResourceList, core::Object);

        AnimationResourceList(const core::string & _name, core::IObject * _parent);
        ~AnimationResourceList();

        void                                        OnEnable                () override;
        void                                        OnDisable               () override;

        bool                                        Add                     () final override;
        bool                                        Remove                  () final override;
        size_t                                      Size                    () const final override;

        const core::vector<AnimationResource> &     getAnimationResources   () const { return m_animationResources; }
        core::vector<AnimationResource> &           getAnimationResources   () { return m_animationResources; }

    private:
        core::vector<AnimationResource>             m_animationResources;
    };
}