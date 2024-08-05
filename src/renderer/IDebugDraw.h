#pragma once

#include "core/Object/Object.h"

namespace vg::core
{
    class IWorld;
}

namespace vg::renderer
{
    class IDebugDraw : public core::Object
    {
    public:
        virtual void    AddLine             (const core::IWorld * _world, const core::float3 & _beginPos, const core::float3 & _endPos, core::u32 _color, const core::float4x4 & _matrix = core::float4x4::identity()) = 0;
        virtual void    AddWireframeBox     (const core::IWorld * _world, const core::float3 & _minPos, const core::float3 & _maxPos, core::u32 _color, const core::float4x4 & _matrix = core::float4x4::identity()) = 0;
        virtual void    AddWireframeSphere  (const core::IWorld * _world, float _radius, core::u32 _color, const core::float4x4 _matrix = core::float4x4::identity()) = 0;
        virtual void    AddHemisphere       (const core::IWorld * _world, float _radius, core::u32 _color, const core::float4x4 _matrix = core::float4x4::identity()) = 0;
        virtual void    AddCylinder         (const core::IWorld * _world, float _radius, float _height, core::u32 _color, const core::float4x4 _matrix = core::float4x4::identity()) = 0;
        virtual void    AddTaperedCylinder  (const core::IWorld * _world, float _topRadius, float _bottomRadius, float _height, core::u32 _color, const core::float4x4 _matrix = core::float4x4::identity()) = 0;
        virtual void    AddCapsule          (const core::IWorld * _world, float _radius, float _height, core::u32 _color, const core::float4x4 _matrix = core::float4x4::identity()) = 0;
        virtual void    AddTaperedCapsule   (const core::IWorld * _world, float _topRadius, float _bottomRadius, float _height, core::u32 _color, const core::float4x4 _matrix = core::float4x4::identity()) = 0;
    };
}