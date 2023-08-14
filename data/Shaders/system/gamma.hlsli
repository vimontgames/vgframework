#pragma once

#if 1
// Precise gamma
float3 Linear2sRGB(float3 value)
{
    return select(value <= 0.0031308f, value * 12.92f, pow(value, 1.0f / 2.4f) * 1.055f - 0.055f);
}
float3 sRGB2Linear(float3 value)
{
    return select(value <= 0.04045f, value / 12.92f, pow((value + 0.055f) / 1.055f, 2.4f));
}
#elif 0
// Average gamma
float3 Linear2sRGB(float3 value)    { return pow(value, 1.0f / 2.2f); }
float3 sRGB2Linear(float3 value)    { return pow(value, 2.2f); }
#else
// Quick and dirty gamma
float3 Linear2sRGB(float3 value)    { return sqrt(value); }
float3 sRGB2Linear(float3 value)    { return value * value; }
#endif

float4 Linear2sRGBA(float4 value)
{
    return float4(Linear2sRGB(value.rgb), 1);
}
float4 sRGBA2Linear(float4 value)
{
    return float4(sRGB2Linear(value.rgb), 1);
}