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
    output.pos = rootConstants.quad.getPosition(_vertexID);

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
    output.color0.rgb = Texture2DTable[rootConstants.texID].Sample(nearestRepeat, uv).rgb;
    //output.color0.rgb = Texture3DTable[index].Sample(nearestClamp, float3(uv,0)).rgb;

    // Uniform buffer
    output.color0 *= asfloat(BufferTable[0].Load4(0)); 

    // Texture UAV
    //output.color0 = RWTexture2DTable[0].Load(0);

    return output;
}



