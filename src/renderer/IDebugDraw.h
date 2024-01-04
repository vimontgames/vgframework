#pragma once

#include "core/Object/Object.h"

namespace vg::renderer
{
    class IDebugDraw : public core::Object
    {
    public:
        struct LineDesc
        {
            inline LineDesc(const core::float3 & _beginPos, const core::float3 & _endPos, core::u32 _color, const core::float4x4 & _world = core::float4x4::identity()) :
                beginPos(_beginPos),
                endPos(_endPos),
                color(_color),
                world(_world)
            {

            }

            const core::float4x4    world = core::float4x4::identity();
            const core::float3      beginPos;
            const core::float3      endPos;
            core::u32               color;
        };

        virtual void    AddLine         (const core::float3 & _beginPos, const core::float3 & _endPos, core::u32 _color, const core::float4x4 & _world = core::float4x4::identity()) = 0;
        virtual void    AddLine         (const LineDesc & _lineDesc) = 0;
        virtual void    AddLines        (const core::vector<LineDesc> & _linesDesc) = 0;

        struct BoxDesc
        {
            inline BoxDesc(const core::float3 & _minPos, const core::float3 & _maxPos, core::u32 _color, const core::float4x4 & _world = core::float4x4::identity()) :
                minPos(_minPos),
                maxPos(_maxPos),
                color(_color),
                world(_world)
            {

            }
            const core::float4x4    world = core::float4x4::identity();
            const core::float3      minPos;
            const core::float3      maxPos;
            core::u32               color; 
        };

        virtual void    AddWireframeBox     (const core::float3 & _minPos, const core::float3 & _maxPos, core::u32 _color, const core::float4x4 & _world = core::float4x4::identity()) = 0;
        virtual void    AddWireframeBox     (const BoxDesc & _boxDesc) = 0;
        virtual void    AddWireframeBoxes   (const core::vector< BoxDesc> & _boxesDesc) = 0;

        virtual void    AddWireframeSphere(float _radius, core::u32 _color, const core::float4x4 _world = core::float4x4::identity()) = 0;
        virtual void    AddHemisphere(float _radius, core::u32 _color, const core::float4x4 _world = core::float4x4::identity()) = 0;
        virtual void    AddCylinder(float _radius, float _height, core::u32 _color, const core::float4x4 _world = core::float4x4::identity()) = 0;
        virtual void    AddCapsule(float _radius, float _height, core::u32 _color, const core::float4x4 _world = core::float4x4::identity()) = 0;
    };
}