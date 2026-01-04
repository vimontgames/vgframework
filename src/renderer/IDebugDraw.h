#pragma once

#include "core/Object/Object.h"

namespace vg::core
{
    class IWorld;
    class IDebugDrawData;
}

namespace vg::renderer
{
    using PickingID = core::uint;

    struct DebugVertex
    {
        core::float3 position;
        core::float3 normal;
        core::float2 uv;
        core::u32 color;
    };

    class IDebugGeometry : public core::Object
    {

    };

    class IDebugDraw : public core::Object
    {
    public:
        virtual core::IDebugDrawData * CreateDebugDrawData() = 0;

        virtual void            AddLine                     (const core::IWorld * _world, const core::float3 & _beginPos, const core::float3 & _endPos, core::u32 _color, const core::float4x4 & _matrix = core::float4x4::identity(), PickingID _pickingID = 0) = 0;
        
        virtual void            AddWireframeCube            (const core::IWorld * _world, const core::float3 & _minPos, const core::float3 & _maxPos, core::u32 _color, const core::float4x4 & _matrix = core::float4x4::identity(), PickingID _pickingID = 0) = 0;
        virtual void            AddWireframeSphere          (const core::IWorld * _world, float _radius, core::u32 _color, const core::float4x4 & _matrix = core::float4x4::identity(), PickingID _pickingID = 0) = 0;
        virtual void            AddWireframeHemisphere      (const core::IWorld * _world, float _radius, core::u32 _color, const core::float4x4 & _matrix = core::float4x4::identity(), PickingID _pickingID = 0) = 0;
        virtual void            AddWireframeCylinder        (const core::IWorld * _world, float _radius, float _height, core::u32 _color, const core::float4x4 & _matrix = core::float4x4::identity(), PickingID _pickingID = 0) = 0;
        virtual void            AddWireframeTaperedCylinder (const core::IWorld * _world, float _topRadius, float _bottomRadius, float _height, core::u32 _color, const core::float4x4 & _matrix = core::float4x4::identity(), PickingID _pickingID = 0) = 0;
        virtual void            AddWireframeCapsule         (const core::IWorld * _world, float _radius, float _height, core::u32 _color, const core::float4x4 & _matrix = core::float4x4::identity(), PickingID _pickingID = 0) = 0;
        virtual void            AddWireframeTaperedCapsule  (const core::IWorld * _world, float _topRadius, float _bottomRadius, float _height, core::u32 _color, const core::float4x4 & _matrix = core::float4x4::identity(), PickingID _pickingID = 0) = 0;
        virtual void            AddWireframeSquarePyramid   (const core::IWorld * _world, float _base, float _height, core::u32 _color, const core::float4x4 & _matrix = core::float4x4::identity(), PickingID _pickingID = 0) = 0;
        virtual void            AddWireframeGeometry        (const core::IWorld * _world, IDebugGeometry * _geometry, core::u32 _color, const core::float4x4 & _matrix = core::float4x4::identity(), PickingID _pickingID = 0) = 0;

        virtual void            AddSolidCube                (const core::IWorld * _world, const core::float3 & _minPos, const core::float3 & _maxPos, core::u32 _color, const core::float4x4 & _matrix = core::float4x4::identity(), PickingID _pickingID = 0) = 0;
        virtual void            AddSolidSphere              (const core::IWorld * _world, float _radius, core::u32 _color, const core::float4x4 & _matrix = core::float4x4::identity(), PickingID _pickingID = 0) = 0;
        virtual void            AddSolidHemisphere          (const core::IWorld * _world, float _radius, core::u32 _color, const core::float4x4 & _matrix = core::float4x4::identity(), PickingID _pickingID = 0) = 0;
        virtual void            AddSolidCylinder            (const core::IWorld * _world, float _radius, float _height, core::u32 _color, const core::float4x4 & _matrix = core::float4x4::identity(), PickingID _pickingID = 0) = 0;
        virtual void            AddSolidSquarePyramid       (const core::IWorld * _world, float _base, float _height, core::u32 _color, const core::float4x4 & _matrix = core::float4x4::identity(), PickingID _pickingID = 0) = 0;
        virtual void            AddSolidGeometry            (const core::IWorld * _world, IDebugGeometry * _geometry, core::u32 _color, const core::float4x4 & _matrix = core::float4x4::identity(), PickingID _pickingID = 0) = 0;

        virtual IDebugGeometry* CreateGeometry              (const DebugVertex * _triangles, core::u32 _triangleCount, const core::u32 * _indices, core::u32 _indexCount) = 0;
        
    };
}