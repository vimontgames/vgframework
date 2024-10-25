#pragma once

#define FXAA_PC 1
#define FXAA_HLSL_5 1
#define FXAA_QUALITY__PRESET 39
#define FXAA_GATHER4_ALPHA 0
#define FXAA_GREEN_AS_LUMA 1

#include "extern/FXAA/Fxaa3_11.h"

float4 FXAA(Texture2D src, float2 uv)
{
    FxaaTex tex = {linearClamp, src};
    float2 fxaaQualityRcpFrame = 1.0f / (float2)postProcessConstants.getScreenSize();
    float fxaaQualitySubpix = 0.75;
    float fxaaQualityEdgeThreshold = 0.125;
    float fxaaQualityEdgeThresholdMin = 0.0312;

    return FxaaPixelShader( uv,                             // pos                              {xy} = center of pixel
                            0,                              // fxaaConsolePosPos                Used only for FXAA Console, and not used on the 360 version.
                            tex,                            // tex                              Input color texture.
                            tex,                            // texLuma                          Input luma texture
                            tex,                            // fxaaConsole360TexExpBiasNegOne   Only used on the optimized 360 version of FXAA Console
                            tex,                            // fxaaConsole360TexExpBiasNegTwo   Only used on the optimized 360 version of FXAA Console.
                            fxaaQualityRcpFrame,            // fxaaQualityRcpFrame              Only used on the optimized 360 version of FXAA Console.
                            0,                              // fxaaConsoleRcpFrameOpt           Only used on FXAA Console.
                            0,                              // fxaaConsoleRcpFrameOpt2          Only used on FXAA Console.
                            0,                              // fxaaConsole360RcpFrameOpt2       Only used on FXAA Console.
                            fxaaQualitySubpix,              // fxaaQualitySubpix                Only used on FXAA Quality.
                            fxaaQualityEdgeThreshold,       // fxaaQualityEdgeThreshold         Only used on FXAA Quality.
                            fxaaQualityEdgeThresholdMin,    // fxaaQualityEdgeThresholdMin      Only used on FXAA Console.
                            0,                              // fxaaConsoleEdgeSharpness         This used to be the FXAA_CONSOLE__EDGE_SHARPNESS define
                            0,                              // fxaaConsoleEdgeThreshold         Only used on FXAA Console
                            0,                              // fxaaConsoleEdgeThresholdMin      Only used on FXAA Console.
                            0);                             // fxaaConsole360ConstDir           Extra constants for 360 FXAA Console only                            
}