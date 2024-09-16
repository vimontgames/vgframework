#pragma once

#include "engine/Resource/ResourceList.h"
#include "engine/Resource/Animation/AnimationResource.h"

namespace vg::engine
{
    class AnimationResourceList : public ResourceList<AnimationResource>
    {
    public:
        VG_CLASS_DECL(AnimationResourceList, ResourceList<AnimationResource>);

        AnimationResourceList(const core::string & _name, core::IObject * _parent);
        ~AnimationResourceList();
    };
}