#include "system/bindless.hlsli"
#include "system/semantics.hlsli"
#include "system/samplers.hlsli"
#include "system/gamma.hlsli"
#include "system/rootConstants2D.hlsli"
#include "system/color.hlsli"

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
    //output.color0.rgb = getTexture1D(rootConstants2D.texID).Sample(nearestClamp, uv.x).rgb;
    output.color0.rgba = getTexture2D(rootConstants2D.texID).Sample(nearestRepeat, uv).rgba;
    //output.color0.rgb = getTexture3D(rootConstants2D.texID).Sample(nearestClamp, float3(uv,0)).rgb;

    // Uniform buffer
    //output.color0 = asfloat(BufferTable[rootConstants.cbID].Load4(0));

    // Read-Write texture
    //output.color0 = RWTexture2DTable[0].Load(0);

    return output;
}

// Constants for PQ curve
static const float M1 = 2610.0 / 16384.0;
static const float M2 = 2523.0 / 4096.0 * 128.0;
static const float C1 = 3424.0 / 4096.0;
static const float C2 = 2413.0 / 4096.0 * 32.0;
static const float C3 = 2392.0 / 4096.0 * 32.0;

// Linear to ST 2048 PQ encoding
float3 LinearToST2048PQ(float3 color)
{
    color = saturate(color);
    float3 numerator = pow(color, M1) * C2 + C1;
    float3 denominator = pow(color, M1) * C3 + 1.0;
    return pow(numerator / denominator, M2);
}

PS_Output_Quad PS_Copy(VS_Output_Quad _input)
{
    PS_Output_Quad output;
    float2 uv = _input.uv;
    output.color0 = float4(uv, 0, 1);

    output.color0.rgba = getTexture2D(rootConstants2D.texID).Sample(nearestRepeat, uv).rgba;

    float standardNits = 80; // temp
    const float st2084max = 10000.0;
    const float hdrScalar = standardNits / st2084max;
    const float hdrBoost = 7.53749 * 0.5f;

    #if _GAMMA
    output.color0.rgb = Linear2sRGB(output.color0.rgb);
    #endif

    #if _HDR10
    output.color0.rgb = SRGBToLinear(output.color0.rgb) * hdrBoost;
    
    // The HDR scene is in Rec.709, but the display is Rec.2020
    output.color0.rgb = Rec709ToRec2020(output.color0.rgb);
    
    // Apply the ST.2084 curve to the scene.
    output.color0.rgb = LinearToST2084(output.color0.rgb * hdrScalar);    

    //output.color0.rgb = output.color0.a;

    #endif

    #if _HDR16
    output.color0.rgb = SRGBToLinear(output.color0.rgb) * hdrBoost;
    #endif

    output.color0.a = 1;

    return output;
}


