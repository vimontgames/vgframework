#include "../system/bindless.hlsli"
#include "../system/semantics.hlsli"
#include "../system/samplers.hlsli"

#include "driver.hlsli"

struct VS_Output_Quad
{
    float4 pos  : Position;
    float2 uv   : Texcoord0;
};

VS_Output_Quad VS_Quad(uint _vertexID : VertexID)
{
    VS_Output_Quad output;

    output.uv = rootConstants.quad.getUV0(_vertexID);
    output.pos = mul(rootConstants.mat, rootConstants.quad.getPosition(_vertexID));

    return output;
}

struct PS_Output_Quad
{
    float4 color0 : Color0;
};

PS_Output_Quad PS_Quad(VS_Output_Quad _input)
{
    PS_Output_Quad output;
    float2 uv = _input.uv;
    output.color0 = float4(uv, 0, 1);

    // RO texture
    //output.color0.rgb = Texture1DTable[index].Sample(nearestClamp, uv.x).rgb;
    output.color0.rgba = Texture2DTable[rootConstants.texID].Sample(nearestRepeat, uv).rgba;
    //output.color0.rgb = Texture3DTable[index].Sample(nearestClamp, float3(uv,0)).rgb;

    // Uniform buffer
    //output.color0 = asfloat(BufferTable[rootConstants.cbID].Load4(0));

    // Constant buffer
    //output.color0 = ubo[rootConstants.cbID].test;

    // Texture UAV
    //output.color0 = RWTexture2DTable[0].Load(0);

    return output;
}

PS_Output_Quad PS_Copy(VS_Output_Quad _input)
{
    PS_Output_Quad output;
    float2 uv = _input.uv;
    output.color0 = float4(uv, 0, 1);

    output.color0.rgba = Texture2DTable[rootConstants.texID].Sample(nearestRepeat, uv).rgba;

    return output;
}



