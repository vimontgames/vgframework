#include "packing.hlsli"

#define SimpleVertex_stride (17 * sizeof(float) >> 2)

#if USE_BYTEADDRESSBUFFER 
float4 LoadBuffer4(ByteAddressBuffer _buffer, uint _offset) { return float4(asfloat(_buffer.Load4(_offset<<2))); }
float3 LoadBuffer3(ByteAddressBuffer _buffer, uint _offset) { return float3(asfloat(_buffer.Load3(_offset<<2))); }
float2 LoadBuffer2(ByteAddressBuffer _buffer, uint _offset) { return float2(asfloat(_buffer.Load2(_offset<<2))); }
float  LoadBuffer (ByteAddressBuffer _buffer, uint _offset)  { return asfloat(_buffer.Load(_offset<<2)); }
#else
float4 LoadBuffer4(Buffer<uint> _buffer, uint _offset)	{ return float4(asfloat(_buffer.Load(_offset)), asfloat(_buffer.Load(_offset + 1)), asfloat(_buffer.Load(_offset + 2)), asfloat( _buffer.Load(_offset + 3)) ); }
float3 LoadBuffer3(Buffer<uint> _buffer, uint _offset)	{ return float3(asfloat(_buffer.Load(_offset)), asfloat(_buffer.Load(_offset + 1)), asfloat(_buffer.Load(_offset + 2))); }
float2 LoadBuffer2(Buffer<uint> _buffer, uint _offset)	{ return float2(asfloat(_buffer.Load(_offset)), asfloat(_buffer.Load(_offset + 1)) ); }
float  LoadBuffer (Buffer<uint> _buffer, uint _offset)	{ return asfloat(_buffer.Load(_offset)); }
#endif

struct SimpleVertex
{
    void load(ByteAddressBuffer _buffer, uint _vertexID, uint _offset = 0)
    {
		uint vertexOffset = _offset + _vertexID * SimpleVertex_stride;

		pos.xyz = LoadBuffer3(_buffer, vertexOffset);
        nrm.xyz = LoadBuffer3(_buffer, vertexOffset + 3);
        bin.xyz = LoadBuffer3(_buffer, vertexOffset + 6);
        tan.xyz = LoadBuffer3(_buffer, vertexOffset + 9);
        uv[0].xy = LoadBuffer2(_buffer, vertexOffset + 12);
        uv[1].xy = LoadBuffer2(_buffer, vertexOffset + 14);
        color = asuint(LoadBuffer(_buffer, vertexOffset + 16));
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