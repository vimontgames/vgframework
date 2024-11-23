#include "system/bindless.hlsli"
#include "system/semantics.hlsli"
#include "system/samplers.hlsli"
#include "system/gamma.hlsli"
#include "system/rootConstants2D.hlsli"
#include "system/color.hlsli"
#include "system/view.hlsli" 
#include "system/msaa.hlsli"
#include "system/depthstencil.hlsli"

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

struct PS_Input_Quad
{
    linear noperspective float4 pos         : Position;
    linear noperspective centroid float2 uv : Texcoord0;    // using centroid here ensures we have the same result on both DX12 and Vulkan

    #if SAMPLE_COUNT > 1
    uint sampleIndex : SV_SampleIndex; 
    #endif
};

struct PS_Output_Quad
{
    float4 color0 : Color0;
};

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

PS_Output_Quad PS_Copy(PS_Input_Quad _input)
{
    PS_Output_Quad output;
    float2 uv = _input.uv;

    #if SAMPLE_COUNT > 1    

    ViewConstants viewConstants;
    viewConstants.Load(getBuffer(RESERVEDSLOT_BUFSRV_VIEWCONSTANTS));

    int2 scale = viewConstants.getScreenSize().xy * getMSAASampleScale(SAMPLE_COUNT);
    int2 offset = getMSAASampleOffset(SAMPLE_COUNT, _input.sampleIndex);

    output.color0 = getTexture2D(rootConstants2D.texID).Load(int3(uv * scale + offset, 0)); 
    
    #else

    output.color0.rgba = getTexture2D(rootConstants2D.texID).Sample(nearestRepeat, uv).rgba;

    #endif

    // Apply HDR 
    float standardNits = 80; // temp
    const float st2084max = 10000.0;
    const float hdrScalar = standardNits / st2084max;
    const float hdrBoost = 7.53749 * 0.5f;

    #if _GAMMA
    output.color0.rgb = Linear2sRGB(output.color0.rgb);
    #endif // _GAMMA

    #if _HDR10
    output.color0.rgb = SRGBToLinear(output.color0.rgb) * hdrBoost;
    
    // The HDR scene is in Rec.709, but the display is Rec.2020
    output.color0.rgb = Rec709ToRec2020(output.color0.rgb);
    
    // Apply the ST.2084 curve to the scene.
    output.color0.rgb = LinearToST2084(output.color0.rgb * hdrScalar);    

    #endif // _HDR10

    #if _HDR16
    output.color0.rgb = SRGBToLinear(output.color0.rgb) * hdrBoost;
    #endif // _HDR16

    output.color0.a = 1;

    return output;
}

struct PS_Output_MinMaxDepth
{
    float2 color0 : Color0;
};

PS_Output_MinMaxDepth PS_CopyLinearDepth(PS_Input_Quad _input)
{
    PS_Output_MinMaxDepth output;
    float2 uv = _input.uv;

    ViewConstants viewConstants;
    viewConstants.Load(getBuffer(RESERVEDSLOT_BUFSRV_VIEWCONSTANTS));

    uint2 address = viewConstants.getScreenSize().xy * uv;

    #if SAMPLE_COUNT > 1  
    float2 minMaxDepth = float2(1,0);
    [unroll]
    for (uint i = 0; i < SAMPLE_COUNT; ++i)
    {
        float depth = loadDepthMSAA(rootConstants2D.texID, address, i);
        minMaxDepth.x = min(minMaxDepth.x, depth);
        minMaxDepth.y = max(minMaxDepth.y, depth);
    }   
    output.color0 = float2(viewConstants.getLinearDepth(minMaxDepth.x), viewConstants.getLinearDepth(minMaxDepth.y));

    #else
    float depth = loadDepth(rootConstants2D.texID, address);
    output.color0 = viewConstants.getLinearDepth(depth).xx;
    #endif

    return output; 
}


