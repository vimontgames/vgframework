
#define SimpleVertex_stride (8 * sizeof(float) >> 2)

#if USE_BYTEADDRESSBUFFER 
float3 LoadBuffer4(ByteAddressBuffer _buffer, uint _offset) { return float4(asfloat(_buffer.Load4(_offset<<2))); }
float3 LoadBuffer3(ByteAddressBuffer _buffer, uint _offset) { return float3(asfloat(_buffer.Load3(_offset<<2))); }
float2 LoadBuffer2(ByteAddressBuffer _buffer, uint _offset) { return float2(asfloat(_buffer.Load2(_offset<<2))); }
float  LoadBuffer (ByteAddressBuffer _buffer, uint _offset)  { return asfloat(_buffer.Load(_offset<<2)); }
#else
float3 LoadBuffer4(Buffer<uint> _buffer, uint _offset)	{ return float4(asfloat(_buffer.Load(_offset)), asfloat(_buffer.Load(_offset + 1)), asfloat(_buffer.Load(_offset + 2)), asfloat( _buffer.Load(_offset + 3)) ); }
float3 LoadBuffer3(Buffer<uint> _buffer, uint _offset)	{ return float3(asfloat(_buffer.Load(_offset)), asfloat(_buffer.Load(_offset + 1)), asfloat(_buffer.Load(_offset + 2))); }
float2 LoadBuffer2(Buffer<uint> _buffer, uint _offset)	{ return float2(asfloat(_buffer.Load(_offset)), asfloat(_buffer.Load(_offset + 1)) ); }
float  LoadBuffer (Buffer<uint> _buffer, uint _offset)	{ return asfloat(_buffer.Load(_offset)); }
#endif

struct SimpleVertex
{
    void load(ByteAddressBuffer _buffer, uint _vertexID, uint _offset = 0)
    {
		pos = float4(LoadBuffer3(_buffer, _offset + _vertexID * SimpleVertex_stride), 1.0f);
        nrm = LoadBuffer3(_buffer, _offset + _vertexID * SimpleVertex_stride + 3);
        uv  = LoadBuffer3(_buffer, _offset + _vertexID * SimpleVertex_stride + 6);
    }

    float4 getPos()             { return pos; }
    float3 getNrm()             { return nrm; }
    float2 getUV(uint _index)   { return uv; }

    float4 pos;
    float3 nrm;
    float2 uv;
};

#define Vertex SimpleVertex