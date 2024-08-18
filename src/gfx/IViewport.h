#pragma once

#include "core/Object/Object.h"

namespace vg::gfx
{
    enum class ViewportTarget : core::u8
    {
        Backbuffer  = 0,
        Game        = 1,
        Editor      = 2
    };
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

    struct CreateViewportParams
    {
        CreateViewportParams()
        {
        }

        CreateViewportParams(ViewportTarget _target, core::uint2 _size) :
            target(_target),
            size(_size)
        {

        }
        ViewportTarget  target = ViewportTarget::Game;
        core::uint2     size = core::uint2(0, 0);
    };

    class IViewport : public core::Object
    {
    public:
        IViewport() {};
        virtual ~IViewport() = default;

        virtual void            SetSize(core::uint2 _size) = 0;
        virtual core::uint2     GetSize() const = 0;        
    };
}