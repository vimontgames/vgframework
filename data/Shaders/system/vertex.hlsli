
#define SimpleVertex_stride 8 * sizeof(float)

struct SimpleVertex
{
    void load(ByteAddressBuffer _buffer, uint _vertexID, uint _offset = 0)
    {
        pos = float4(asfloat(_buffer.Load3(_offset + _vertexID * SimpleVertex_stride)), 1.0f);
        nrm =        asfloat(_buffer.Load3(_offset + _vertexID * SimpleVertex_stride + 3 * sizeof(float)));
        uv  =        asfloat(_buffer.Load2(_offset + _vertexID * SimpleVertex_stride + 6 * sizeof(float)));
    }

    float4 getPos()             { return pos; }
    float3 getNrm()             { return nrm; }
    float2 getUV(uint _index)   { return uv; }

    float4 pos;
    float3 nrm;
    float2 uv;
};

#define Vertex SimpleVertex