#pragma once

#include "types.hlsli"
#include "packing.hlsli"

// List every vertex format here (max 255 because it's serialized as u8 and 0xFF stands for "invalid vertex format")
enum class VertexFormat : uint
{
    Default         = 0,
    Skinning_4Bones = 1
};

struct Vertex
{
    uint getVertexFormatStride(VertexFormat _format)
    {
        switch (_format)
        {
            default:            
            case VertexFormat::Default:
                return 68;
            
            case VertexFormat::Skinning_4Bones:
                return 100;
        }
    }
    
#ifndef __cplusplus
    void Load(ByteAddressBuffer _buffer, VertexFormat _format, uint _vertexID, uint _offset = 0)
    {
        uint vertexOffset = _offset + _vertexID * getVertexFormatStride(_format);

        pos.xyz     = _buffer.Load<float3>(vertexOffset + 0);
        nrm.xyz     = _buffer.Load<float3>(vertexOffset + 12);
        bin.xyz     = _buffer.Load<float3>(vertexOffset + 24);
        tan.xyz     = _buffer.Load<float3>(vertexOffset + 36);
        uv[0].xy    = _buffer.Load<float2>(vertexOffset + 48);
        uv[1].xy    = _buffer.Load<float2>(vertexOffset + 56);
        color       = _buffer.Load<uint>(vertexOffset + 64);
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