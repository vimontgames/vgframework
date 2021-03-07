#include "../system/bindless.hlsli"
#include "../system/semantics.hlsli"
#include "../system/samplers.hlsli"
#include "../system/vertex.hlsli"

#include "default.hlsli"

struct VS_Output
{
    float4 pos  : Position;
    float3 nrm  : Normal;
    float2 uv   : Texcoord0;
};

VS_Output VS_Forward(uint _vertexID : VertexID)
{
    VS_Output output;

    Vertex vert;
           vert.load(getBuffer(rootConstants3D.data.x), _vertexID);

    output.pos = vert.getPos();
    output.nrm = vert.getNrm();
    output.uv = vert.getUV(0);

    output.pos = mul(float4(output.pos.xyz, 1.0f), rootConstants3D.mat);

    return output;
}

struct PS_Output
{
    float4 color0 : Color0;
};

PS_Output PS_Forward(VS_Output _input)
{
    PS_Output output;
    float2 uv = _input.uv;
    
    output.color0.rgba = Texture2DTable[rootConstants3D.data.y].Sample(nearestRepeat, uv).rgba;

    //output.color0 = float4(uv, 0, 1);
    output.color0 = float4(_input.nrm, 1.0f);
    
    return output;
}

PS_Output PS_Wireframe(VS_Output _input)
{
    PS_Output output;
    output.color0 = float4(0, 1, 0, 1);
    return output;
}
