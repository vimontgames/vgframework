#pragma once

#include "types.hlsli"

inline uint packRGBA8(const float4 _color)
{
    uint r = clamp(uint(_color.r * 255.5f), uint(0), uint(255));
    uint g = clamp(uint(_color.g * 255.5f), uint(0), uint(255));
    uint b = clamp(uint(_color.b * 255.5f), uint(0), uint(255));
    uint a = clamp(uint(_color.a * 255.5f), uint(0), uint(255));

    return (a << 24) | (b << 16) | (g << 8) | r; 
}

inline float4 unpackRGBA8(uint _value)
{
    return float4(_value & 0xFF, (_value >> 8) & 0xFF, (_value >> 16) & 0xFF, (_value >> 24) & 0xFF) / 255.5f;
}

inline uint packUint16(uint2 _value)
{
    return _value.y << 16 | _value.x;
}

inline uint2 unpackUint16(uint _value)
{
    return uint2(0xFFFF & _value, _value >> 16);
}
