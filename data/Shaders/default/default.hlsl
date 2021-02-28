#include "../system/bindless.hlsli"
#include "../system/semantics.hlsli"
#include "../system/samplers.hlsli"

#include "default.hlsli"

struct VS_Output
{
    float4 pos  : Position;
    float2 uv   : Texcoord0;
};

VS_Output VS_Forward(uint _vertexID : VertexID)
{
    VS_Output output;

    float2 tmp = float2(_vertexID & 1, (_vertexID & 2) >> 1);

    output.uv = tmp;
   
    output.pos = float4(0.25f * (tmp.xy*2.0f-1.0f), 0, 1);

    output.pos = mul(output.pos, rootConstants3D.mat);

    //output.pos.w = 1;

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
    output.color0 = float4(uv, 0, 1);

    // RO texture
    //output.color0.rgb = Texture1DTable[index].Sample(nearestClamp, uv.x).rgb;
    //output.color0.rgba = Texture2DTable[rootConstants2D.texID].Sample(nearestRepeat, uv).rgba;
    //output.color0.rgb = Texture3DTable[index].Sample(nearestClamp, float3(uv,0)).rgb;

    // Uniform buffer
    //output.color0 = asfloat(BufferTable[rootConstants.cbID].Load4(0));

    // Constant buffer
    //output.color0 = ubo[rootConstants.cbID].test;

    // Texture UAV
    //output.color0 = RWTexture2DTable[0].Load(0);

    return output;
}
