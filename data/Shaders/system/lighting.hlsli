#pragma once

struct LightingResult
{
    float3 diffuse;
    float3 specular;
};

LightingResult ComputeLighting(float3 _worldNormal)
{
    LightingResult result = (LightingResult)0;

    // fake shitty lighting
    float3 lightDir = normalize(float3(-2, 1,-3));

    result.diffuse = saturate(0.1f + saturate(dot(_worldNormal, -lightDir) * 0.8f));
    result.specular = 0.01f;

    return result;
}

float3 ApplyLighting(float3 _albedo, LightingResult _lighting)
{
    return _albedo.rgb * _lighting.diffuse + _lighting.specular;
}