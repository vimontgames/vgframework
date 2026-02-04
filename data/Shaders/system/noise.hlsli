#ifndef _NOISE__HLSLI_
#define _NOISE__HLSLI_

#include "system/bindless.hlsli"
#include "system/samplers.hlsli"

#define BLUENOISE_TEXTURE_WIDTH     128
#define BLUENOISE_TEXTURE_HEIGHT    128
#define BLUENOISE_TEXTUDE_SLICES    32

//--------------------------------------------------------------------------------------
float2 getBlueNoise(float2 coords, uint frame)
{
    frame = frame % (BLUENOISE_TEXTUDE_SLICES-1);
    uint index = floor(frame);
                
    return getTexture2D(RESERVEDSLOT_TEXSRV_BLUE_NOISE_FIRST + index).SampleLevel(nearestRepeat, coords / (float)BLUENOISE_TEXTURE_WIDTH, 0).rg * 2.0f - 1.0f;
}

//--------------------------------------------------------------------------------------
float2 getBlueNoiseSmooth(float2 coords, float t)
{
    float time = t * 60.0f;
    float frame = time % (float)(BLUENOISE_TEXTUDE_SLICES-1);
    uint indexA = floor(frame);
    uint indexB = ceil(frame);
    float blend = frac(frame);
                
    float2 A = getTexture2D(RESERVEDSLOT_TEXSRV_BLUE_NOISE_FIRST + indexA).SampleLevel(nearestRepeat, coords / (float)BLUENOISE_TEXTURE_WIDTH, 0).rg;
    float2 B = getTexture2D(RESERVEDSLOT_TEXSRV_BLUE_NOISE_FIRST + indexB).SampleLevel(nearestRepeat, coords / (float)BLUENOISE_TEXTURE_HEIGHT, 0).rg;
                
    return lerp(A, B, blend).rg * 2.0f - 1.0f;
}

//--------------------------------------------------------------------------------------
float Hash12(uint2 p)
{
    uint h = p.x * 1664525u + p.y * 1013904223u;
    return frac(h * 0.0000001192092896f);
}

#endif // _NOISE__HLSLI_