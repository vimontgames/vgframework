#ifndef __PACKING_HLSLI__
#define __PACKING_HLSLI__

#include "types.hlsli"

inline uint packR8(uint _packed, uint _value)
{
    return (_packed & 0xFFFFFF00) | _value; 
}

inline uint unpackR8(uint _packed)
{
    return 0x000000FF & _packed;
}

inline uint packG8(uint _packed, uint _value)
{
    return (_packed & 0xFFFF00FF) | (_value<<8); 
}

inline uint unpackG8(uint _packed)
{
    return (0x0000FF00 & _packed) >> 8;
}

inline uint packB8(uint _packed, uint _value)
{
    return (_packed & 0xFF00FFFF) | (_value<<16); 
}

inline uint unpackB8(uint _packed)
{
    return (0x00FF0000 & _packed) >> 16;
}

inline uint packA8(uint _packed, uint _value)
{
    return (_packed & 0x00FFFFFF) | (_value<<24); 
}

inline uint unpackA8(uint _packed)
{
    return (0xFF000000 & _packed) >> 24; // 0xFF000000 mask is unnecessary, will the compiler remove it?
}

// Pack uint with mask 'mask' at bit position 'shift'
inline uint packUint(uint _packed, uint _value, uint mask, uint shift)
{
    return (_packed & (~(mask << shift))) | ((_value & mask) << shift);
}

inline uint unpackUint(uint _packed, uint mask, uint shift)
{
    return (_packed >> shift) & mask;
}

inline uint packRGBA8(float4 _color)
{
    uint4 color = clamp((uint4)(_color * 255.5f), (uint4)0, (uint4)255);
    return (color.a << 24) | (color.b << 16) | (color.g << 8) | color.r; 
}

inline uint packRGBA8(float _r, float _g, float _b, float _a)
{
    uint r = clamp(uint(_r * 255.5f), uint(0), uint(255));
    uint g = clamp(uint(_g * 255.5f), uint(0), uint(255));
    uint b = clamp(uint(_b * 255.5f), uint(0), uint(255));
    uint a = clamp(uint(_a * 255.5f), uint(0), uint(255));

    return (a << 24) | (b << 16) | (g << 8) | r; 
}

inline float4 unpackRGBA8(uint _value)
{
    return float4(_value & 0xFF, (_value >> 8) & 0xFF, (_value >> 16) & 0xFF, (_value >> 24) & 0xFF) / 255.0f;
}

//--------------------------------------------------------------------------------------
// RGBE is a compact 32-bit HDR color format that stores R, G, B as 8-bit mantissas and 
// a shared 8-bit exponent (E). All three color channels share the same exponent, which 
// makes the format very small and extremely fast to unpack. It preserves large dynamic
// range while keeping reasonable precision and very low decoding cost.
// Unpack cost: 10 ALUs
//--------------------------------------------------------------------------------------
inline uint packRGBE(float3 _color)
{
    // clamp input
    _color = max(0, _color);
    
    // max 
    float m = max(max(_color.r, _color.g), _color.b);

    if (m < 1e-32f)
        return 0u;

    // biased exponent
    float e = ceil(log2(m));
    float scale = exp2(-e);  // = 1.0 / 2^e

    uint R = clamp(uint(_color.r * scale * 255.0f + 0.5f), 0u, 255u);
    uint G = clamp(uint(_color.g * scale * 255.0f + 0.5f), 0u, 255u);
    uint B = clamp(uint(_color.b * scale * 255.0f + 0.5f), 0u, 255u);

    uint E = uint(e + 128.0f); // bias exponent to 0–255

    return (E << 24) | (B << 16) | (G << 8) | R;
}

inline float3 unpackRGBE(uint v)
{
    float R = float(v & 0xFF);
    float G = float((v >> 8) & 0xFF);
    float B = float((v >> 16) & 0xFF);
    uint  E = (v >> 24) & 0xFF;

    if (E == 0u)
        return float3(0, 0, 0);

    float scale = exp2(float(E) - 128.0f) / 255.0f;

    return float3(R, G, B) * scale;
}

//--------------------------------------------------------------------------------------
// Pack/Unpack two u16 into one u32
//--------------------------------------------------------------------------------------
inline uint packUint16(uint2 _value)
{
    return _value.y << 16 | _value.x;
}

inline uint2 unpackUint16(uint _value)
{
    return uint2(0xFFFF & _value, _value >> 16);
}

inline uint packUint16low(uint _packed, uint _value)
{
    return (_packed & 0xFFFF0000) | _value;
}

inline uint packUint16high(uint _packed, uint _value)
{
    return (_packed & 0x0000FFFF) | (_value<<16);
}

inline uint unpackUint16low(uint _packed)
{
    return _packed & 0xFFFF;
}

inline uint unpackUint16high(uint _packed)
{
    return (_packed>>16);
}

#endif // __PACKING_HLSLI__
