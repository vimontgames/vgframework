#ifndef _VERTEX__HLSLI_
#define _VERTEX__HLSLI_

#include "packing.hlsli"

#define SimpleVertex_stride (17 * sizeof(float))

struct SimpleVertex
{
    void Load(ByteAddressBuffer _buffer, uint _vertexID, uint _offset = 0)
    {
		uint vertexOffset = _offset + _vertexID * SimpleVertex_stride; 

        pos.xyz     = _buffer.Load<float3>(vertexOffset + 0);
        nrm.xyz     = _buffer.Load<float3>(vertexOffset + 12);
        bin.xyz     = _buffer.Load<float3>(vertexOffset + 24);
        tan.xyz     = _buffer.Load<float3>(vertexOffset + 36);
        uv[0].xy    = _buffer.Load<float2>(vertexOffset + 48);
        uv[1].xy    = _buffer.Load<float2>(vertexOffset + 56);
        color       = _buffer.Load<uint>(vertexOffset + 64);
    }

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

#define Vertex SimpleVertex

#endif 