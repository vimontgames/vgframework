#pragma once

#include "types.hlsli"
#include "packing.hlsli"

// List every vertex format here (max 255 because it's serialized as u8 and 0xFF stands for "invalid vertex format")
enum class VertexFormat : uint
{
    Default         = 0,
    Skinning_4Bones = 1,
    DebugDraw       = 2,
};

inline uint getVertexFormatStride(VertexFormat _format)
{
    switch (_format)
    {
        case VertexFormat::DebugDraw:
            return 16;
        
        default:
        case VertexFormat::Default:
            return 68;
            
        case VertexFormat::Skinning_4Bones:
            return 100;
    }
}

inline bool hasNormal(VertexFormat _format)
{
    switch (_format)
    {
        case VertexFormat::Default:
        case VertexFormat::Skinning_4Bones:
            return true;
        
        default:
        case VertexFormat::DebugDraw:
            return false;
    }
}

inline bool hasBinormal(VertexFormat _format)
{
    switch (_format)
    {
        case VertexFormat::Default:
        case VertexFormat::Skinning_4Bones:
            return true;
        
        default:
        case VertexFormat::DebugDraw:
            return false;
    }
}

inline bool hasTangent(VertexFormat _format)
{
    switch (_format)
    {
        case VertexFormat::Default:
        case VertexFormat::Skinning_4Bones:
            return true;
        
        default:
        case VertexFormat::DebugDraw:
            return false;
    }
}

inline bool hasUV0(VertexFormat _format)
{
    switch (_format)
    {
        case VertexFormat::Default:
        case VertexFormat::Skinning_4Bones:
            return true;
        
        default:
        case VertexFormat::DebugDraw:
            return false;
    }
}

inline bool hasUV1(VertexFormat _format)
{
    switch (_format)
    {
        case VertexFormat::Default:
        case VertexFormat::Skinning_4Bones:
            return true;
        
        default:
        case VertexFormat::DebugDraw:
            return false;
    }
}

inline bool hasColor(VertexFormat _format)
{
    switch (_format)
    {
        case VertexFormat::Default:
        case VertexFormat::Skinning_4Bones:
        case VertexFormat::DebugDraw:
            return true;
        
        default:
            return false;
    }
}

struct Vertex
{    
#ifndef __cplusplus
    void Load(ByteAddressBuffer _buffer, VertexFormat _format, uint _vertexID, uint _offset = 0)
    {
        uint offset = _offset + _vertexID * getVertexFormatStride(_format);
        
        // "memset"
        pos = float3(0, 0, 0);
        nrm = float3(0, 0, 1);
        bin = float3(0, 1, 0);
        tan = float3(1, 0, 0);
        uv[0] = float2(0, 0);
        uv[1] = float2(1, 1);
        color = 0xFFFFFFFF;

        pos.xyz = _buffer.Load<float3>(offset); 
        offset += 3 * sizeof(float);
        
        if (hasNormal(_format))
        {
            nrm.xyz = _buffer.Load<float3>(offset);
            offset += 3 * sizeof(float);
        }
        
        if (hasBinormal(_format))
        {
            bin.xyz = _buffer.Load<float3>(offset);
            offset += 3 * sizeof(float);
        }
        
        if (hasTangent(_format))
        {
            tan.xyz = _buffer.Load<float3>(offset);
            offset += 3 * sizeof(float);
        }
        
        if (hasUV0(_format))
        {
            uv[0].xy= _buffer.Load<float2>(offset);
            offset += 2 * sizeof(float);
        }
        
        if (hasUV1(_format))
        {
            uv[1].xy    = _buffer.Load<float2>(offset);
            offset += 2 * sizeof(float);
        }
        
        if (hasColor(_format))
        {
            color = _buffer.Load<uint>(offset);
            offset += 4;
        }
    }
    
    void Store(RWByteAddressBuffer _rwbuffer, VertexFormat _format, uint _vertexID, uint _offset = 0)
    {
        uint offset = _offset + _vertexID * getVertexFormatStride(_format);
        
        _rwbuffer.Store<float3>(offset, pos.xyz); 
        offset += 3 * sizeof(float);
        
        if (hasNormal(_format))
        {
            _rwbuffer.Store<float3>(offset, nrm.xyz);
            offset += 3 * sizeof(float);
        }
        
        if (hasBinormal(_format))
        {
            _rwbuffer.Store<float3>(offset, bin.xyz);
            offset += 3 * sizeof(float);
        }
        
        if (hasTangent(_format))
        {
            _rwbuffer.Store<float3>(offset, tan.xyz);
            offset += 3 * sizeof(float);
        }
        
        if (hasUV0(_format))
        {
            _rwbuffer.Store<float2>(offset, uv[0].xy);
            offset += 2 * sizeof(float);
        }
        
        if (hasUV1(_format))
        {
            _rwbuffer.Store<float2>(offset, uv[1].xy);
            offset += 2 * sizeof(float);
        }
        
        if (hasColor(_format))
        {
            _rwbuffer.Store<uint>(offset, color);
            offset += 4;
        }
    }
    #endif

    float3 getPos()             { return pos; }
    float3 getNrm()             { return nrm; }
    float3 getBin()             { return bin; }
    float3 getTan()             { return tan; }
    float2 getUV(uint _index)   { return uv[_index]; }
    float4 getColor()           { return unpackRGBA8(color); }

    float3 pos;
    float3 nrm;
    float3 bin;
    float3 tan;
    float2 uv[2];
    uint color;
};