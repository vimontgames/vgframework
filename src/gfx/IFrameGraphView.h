#pragma once

#include "core/Object/Object.h"
#include "core/IWorld.h"

struct PickingData;
struct PickingHit;

namespace vg::core
{
    class IGameObject;
}

namespace vg::gfx
{
    vg_enum_class(vg::gfx, ViewTarget, core::u8,
        Game        = 0,
        Editor      = 1,
        Shadow      = 2
    );
    static inline const ViewTarget ViewTargetInvalid = (ViewTarget)0x03;

    using ViewIndex = core::u8;
    static inline const ViewIndex ViewIndexInvalid = (ViewIndex)0x3F;

    struct ViewID
    {
        ViewID(ViewTarget _target = ViewTargetInvalid, ViewIndex _index = ViewIndexInvalid) :
            target(_target),
            index(_index)
        {
            
        }

        union
        {
            struct
            {
                ViewTarget target : 2;
                ViewIndex index : 6;
            };
            core::u8 id;
        };

        inline bool operator == (const ViewID & _other) const
        {
            return target == _other.target && index == _other.index;
        }

        inline bool operator != (const ViewID & _other) const
        {
            return !(*this == _other);
        }
    };

    class IFrameGraphView : public core::Object
    {
    public:

        IFrameGraphView() {};
        virtual ~IFrameGraphView() = default;

        virtual void    SetViewID   (ViewID _viewID) = 0;
        virtual ViewID  GetViewID   () const = 0;
    };
}