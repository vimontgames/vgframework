#pragma once

#include "core/Object/Object.h"

namespace vg::renderer
{
    class IDebugDraw : public core::Object
    {
    public:
        virtual void    AddLine(const core::float3 & _beginPos, const core::float3 & _endPos, core::u32 _color, const core::float4x4 & _world = core::float4x4::identity()) = 0;
        virtual void    AddWireframeBox(const core::float3 & _minPos, const core::float3 & _maxPos, core::u32 _color, const core::float4x4 & _world = core::float4x4::identity()) = 0;
    };
}