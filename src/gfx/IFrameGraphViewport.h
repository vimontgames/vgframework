#pragma once

#include "core/Object/Object.h"
#include "gfx/IFrameGraphView.h"

vg_enum_class_ns(vg::gfx, ViewportTarget, core::u8,
    Game = 0,
    Editor = 1
);

namespace vg::gfx
{
    static inline const ViewportTarget ViewportTargetInvalid = (ViewportTarget)-1;

    using ViewportIndex = core::u8;
    static inline const ViewportIndex ViewportIndexInvalid = (ViewportIndex)-1;

    struct ViewportID
    {
        ViewportID(ViewportTarget _target = ViewportTargetInvalid, ViewportIndex _index = ViewportIndexInvalid) :
            target(_target),
            index(_index)
        {

        }

        union
        {
            struct
            {
                ViewportTarget target : 2;
                ViewportIndex index : 6;
            };
            core::u8 id;
        };

        inline bool operator == (const ViewportID & _other) const
        {
            return target == _other.target && index == _other.index;
        }

        inline bool operator != (const ViewportID & _other) const
        {
            return !(*this == _other);
        }
    };

    class IFrameGraphViewport : public core::Object
    {
    public:

        IFrameGraphViewport() {};
        virtual ~IFrameGraphViewport() = default;
    };
}