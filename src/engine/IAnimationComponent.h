#pragma once

#include "core/Component/Component.h"

namespace vg::engine
{
    class IAnimationResource;
    enum class AnimationFlags : core::u32;

    class IAnimationComponent : public core::Component
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IAnimationComponent, core::Component);

        virtual IAnimationResource *    GetAnimation        (core::uint _index) const = 0;
        virtual core::uint              GetAnimationIndex   (const core::string & _name) const = 0;
        virtual core::uint              GetAnimationCount   () const = 0;

        virtual bool                    PlayAnimation       (core::uint _index, bool _loop) = 0;
        virtual bool                    StopAnimation       (core::uint _index) = 0;
    };
}