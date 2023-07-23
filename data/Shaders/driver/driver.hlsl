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

    output.uv = rootConstants2D.quad.getUV0(_vertexID);
    output.pos = rootConstants2D.quad.getPosition(_vertexID); // mul(rootConstants.mat, rootConstants.quad.getPosition(_vertexID));

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
    output.color0.rgba = Texture2DTable[rootConstants2D.texID].Sample(nearestRepeat, uv).rgba;
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

    output.color0.rgba = Texture2DTable[rootConstants2D.texID].Sample(nearestRepeat, uv).rgba;

    return output;
}

#if 1

float3 Linear2sRGB(float3 value)
{
    return value <= 0.0031308f ? value * 12.92f : pow(value, 1.0f / 2.4f) * 1.055f - 0.055f;
}

float3 sRGB2Linear(float3 value)
{
    return value <= 0.04045f ? value / 12.92f : pow((value + 0.055f) / 1.055f, 2.4f);
}

#elif 0

float3 Linear2sRGB(float3 value)
{
    return pow(value, 1.0f / 2.2f);
}

float3 sRGB2Linear(float3 value)
{
    return pow(value, 2.2f);
}

#else

float3 Linear2sRGB(float3 value)
{
    return sqrt(value);
}

float3 sRGB2Linear(float3 value)
{
    return value * value;
}

#endif

PS_Output_Quad PS_Gamma(VS_Output_Quad _input)
{
    PS_Output_Quad output;
    float2 uv = _input.uv;
    output.color0 = float4(uv, 0, 1);

    output.color0.rgba = Texture2DTable[rootConstants2D.texID].Sample(nearestRepeat, uv).rgba;
    output.color0.rgb = Linear2sRGB(output.color0.rgb); // pow(output.color0.rgb, 1.0f / 2.2f);
    output.color0.a = 1; // Should be an option

    return output;
}



