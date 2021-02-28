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

    output.pos.xyz = asfloat(getBuffer(rootConstants3D.data.x).Load3(_vertexID * 4 * 5));
    output.uv      = asfloat(getBuffer(rootConstants3D.data.x).Load2(_vertexID * 4 * 5 + 3 * 4));

    output.pos = mul(float4(output.pos.xyz, 1.0f), rootConstants3D.mat);

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
    output.color0.rgba = Texture2DTable[rootConstants3D.data.y].Sample(nearestRepeat, uv).rgba;
    //output.color0.rgb = Texture3DTable[index].Sample(nearestClamp, float3(uv,0)).rgb;

    // Uniform buffer
    //output.color0 = asfloat(BufferTable[rootConstants.cbID].Load4(0));

    // Constant buffer
    //output.color0 = ubo[rootConstants.cbID].test;

    // Texture UAV
    //output.color0 = RWTexture2DTable[0].Load(0);

    output.color0 = getTexture2D(rootConstants3D.data.y).Sample(nearestClamp, uv.xy);

    //output.color0 = asfloat(getBuffer(rootConstants3D.data.x).Load4(0));

    return output;
}

PS_Output PS_Wireframe(VS_Output _input)
{
    PS_Output output;
    output.color0 = float4(0, 1, 0, 1);
    return output;
}
