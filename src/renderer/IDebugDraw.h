#pragma once

#include "core/Object/Object.h"

namespace vg::renderer
{
    class IDebugDraw : public core::Object
    {
    public:
        virtual void    AddLine(const core::float3 & _beginPos, const core::float3 & _endPos, core::u32 _color) = 0;
    };
}