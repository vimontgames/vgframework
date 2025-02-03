#pragma once

#include "system/constants.hlsli"
#include "system/packing.hlsli"

struct DebugDrawRootConstants3D
{
    float4x4    world;          // world matrix (TODO: pass float3x4?)
    float4      color;
    uint        stream0;        // unused (16) | stream0 address (16)
    uint        stream0Offset;  // stream0 offset (might need more than 16 bits because of skinning)
    uint        misc;           // unused (16) | format (8) | unused (8)
    float       taper;          // Top/Bottom ratio for tapered shapes
    uint        picking;        // PickingID

    #ifdef __cplusplus
    DebugDrawRootConstants3D() :
        world(float4x4::identity()),
        color(float4(1,1,1,1)),
        stream0(0x0000FFFF),
        stream0Offset(0),
        taper(1.0f),
        picking(0)
    {

    }
    #endif

    void setWorldMatrix(float4x4 _world)
    {
        world = _world;
    }
    
    float4x4 getWorldMatrix()
    {
        return world;
    }

    void setColor(float4 _color)
    {
        color = _color;
    }
    
    float4 getColor()
    {
        return color;
    }

    void setTaper(float _taper)
    {
       taper = _taper;
    }
    
    float getTaper()
    {
        return taper;
    }

    void setVertexBufferHandle(uint _stream0, uint _offset = 0)
    {
        stream0 = packUint16low(stream0, _stream0);
        stream0Offset = _offset / GPU_VERTEXBUFFER_OFFSET_ALIGNMENT;
    }

    uint getVertexBufferHandle()
    {
        return unpackUint16low(stream0);
    }
    
    uint getVertexBufferOffset()
    {
        return stream0Offset * GPU_VERTEXBUFFER_OFFSET_ALIGNMENT;
    }

    void setVertexFormat(VertexFormat _value)
    {
        misc = (misc & ~0x00FF0000UL) | ((((uint) _value) & 0xFF) << 16);
    }

    VertexFormat getVertexFormat()
    {
        return (VertexFormat) ((misc >> 16) & 0xFF);
    }

    void setPickingID(uint _value)
    {
        picking = _value;
    }
    
    uint getPickingID()
    {
        return picking;
    }
};

#define DebugDrawRootConstants3DCount sizeof(DebugDrawRootConstants3D)/sizeof(u32)

#ifndef __cplusplus
DECL_ROOTCONSTANTS(DebugDrawRootConstants3D, debugDrawRootConstants3D, 0, 0);
#endif