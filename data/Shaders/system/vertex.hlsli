#include "packing.hlsli"
#include "buffer.hlsli"

#define SimpleVertex_stride (17 * sizeof(float) >> 2)

struct SimpleVertex
{
    void load(ByteAddressBuffer _buffer, uint _vertexID, uint _offset = 0)
    {
		uint vertexOffset = _offset + _vertexID * SimpleVertex_stride;

		Load(pos.xyz, _buffer, vertexOffset);
        Load(nrm.xyz, _buffer, vertexOffset + 3);
        Load(bin.xyz, _buffer, vertexOffset + 6);
        Load(tan.xyz, _buffer, vertexOffset + 9);
        Load(uv[0].xy, _buffer, vertexOffset + 12);
        Load(uv[1].xy, _buffer, vertexOffset + 14);
        Load(color, _buffer, vertexOffset + 16);
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