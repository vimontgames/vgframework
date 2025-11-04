#pragma once

#include "core/Component/Component.h"

namespace vg::engine
{
    class IUIComponent : public core::Component
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IUIComponent, core::Component)

        virtual core::uint2     GetSize     () const = 0;
        virtual void            SetSize     (const core::uint2 _size) = 0;

        virtual core::float4    GetColor    () const = 0;
        virtual void            SetColor    (const core::float4 & _color) = 0;
    };
}