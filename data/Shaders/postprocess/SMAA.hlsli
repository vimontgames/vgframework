#pragma once

float4 SMAA(Texture2D _tex, uint3 _coord)
{
    // TODO
    float4 color = _tex.Load(_coord);
    color.rgb = dot(color.rgb, 0.5f);
    return color.rgba;
}