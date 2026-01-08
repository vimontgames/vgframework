#pragma once

#include "core/Types/Types.h"
#include "StorageVector.h"
#include "CRC.h"

#include <cfloat>

namespace vg::core
{
    const float PI = 3.14159265359f;

    const float MIN_FLOAT = FLT_MIN;
    const float MAX_FLOAT = FLT_MAX;

    //--------------------------------------------------------------------------------------
    inline float asfloat(uint i)
    {
        union 
        {
            uint u;
            float f;
        } converter;

        converter.u = i;
        return converter.f;
    }

    //--------------------------------------------------------------------------------------
    inline uint asuint(float f)
    {
        union
        {
            uint u;
            float f;
        } converter;

        converter.f = f;
        return converter.u;
    }

    //--------------------------------------------------------------------------------------
    inline core::u32 getU32Color(core::float4 _color)
    {
        core::uint4 color = clamp((core::uint4)(_color * 255.5f), (core::uint4)0, (core::uint4)255);
        return (color.a << 24) | (color.b << 16) | (color.g << 8) | color.r;
    }

    //--------------------------------------------------------------------------------------
    inline float4x4 clearRotation(const float4x4 & _matrix)
    {
        float3 scale;
        scale.x = length(_matrix[0].xyz);
        scale.y = length(_matrix[1].xyz);
        scale.z = length(_matrix[2].xyz);

        float4x4 matrix;
        matrix[0] = float4(scale.x, 0, 0, 0);
        matrix[1] = float4(0, scale.y, 0, 0);
        matrix[2] = float4(0, 0, scale.z, 0);
        matrix[3] = _matrix[3];
        return matrix;
    }

    //--------------------------------------------------------------------------------------
    inline float4x4 clearScale(const float4x4 & _matrix)
    {
        float4x4 matrix;
        matrix[0] = float4(normalize(_matrix[0].xyz), 0);
        matrix[1] = float4(normalize(_matrix[1].xyz), 0);
        matrix[2] = float4(normalize(_matrix[2].xyz), 0);
        matrix[3] = _matrix[3];
        return matrix;
    }

    //--------------------------------------------------------------------------------------
    inline float4x4 clearRotationAndScale(const float4x4 & _matrix)
    {
        float4x4 matrix;
        matrix[0] = float4(1, 0, 0, 0);
        matrix[1] = float4(0, 1, 0, 0);
        matrix[2] = float4(0, 0, 1, 0);
        matrix[3] = _matrix[3];
        return matrix;
    }

    //--------------------------------------------------------------------------------------
    inline float4x4 clearTranslation(const float4x4 & _matrix)
    {
        float4x4 matrix;
        matrix[0] = _matrix[0];
        matrix[1] = _matrix[1];
        matrix[2] = _matrix[2];
        matrix[3] = float4(0,0,0,1);
        return matrix;
    }

    //--------------------------------------------------------------------------------------
    template <typename T> T inline sign(T _value)
    {
        return (_value > T(0)) ? T(+1) : ((_value < T(0)) ? T(-1) : T(0));
    }

    //--------------------------------------------------------------------------------------
    template <typename T> T inline signnz(T _value)
    {
        return (_value >= T(0)) ? T(+1) : T(-1);
    }

    //--------------------------------------------------------------------------------------
    inline float degreesToRadians(float _degrees)
    {
        return _degrees * (PI / 180.0f);
    }

    //--------------------------------------------------------------------------------------
    inline float radiansToDegrees(float _radians)
    {
        return _radians * (180.0f / PI);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void Float4x4ToTRS(const float4x4 & _mat44, float3 & _translation, float3 & _rotation, float3 & _scale);
    
    //--------------------------------------------------------------------------------------
    VG_INLINE float4x4 TRSToFloat4x4(const float3 & _translation, const float3 & _rotation, const float3 & _scale);

    //--------------------------------------------------------------------------------------
    VG_INLINE float4x4 TRSToFloat4x4(const float3 & _translation, const quaternion _rotation, const float3 _scale);

    //--------------------------------------------------------------------------------------
    float3x3 extractRotation(const float4x4 & _matrix);

	//--------------------------------------------------------------------------------------
	template <typename T> inline T min(const T & _left, const T & _right)
	{
		return _left < _right ? _left : _right;
	}

	//--------------------------------------------------------------------------------------
	template <typename T> inline T max(const T & _left, const T & _right)
	{
		return _left > _right ? _left : _right;
	}

    //--------------------------------------------------------------------------------------
    template <class T> inline T clamp(T _value, T _min, T _max)
    {
        return (_value < _min) ? _min : ((_value > _max) ? _max : _value);
    }

    //--------------------------------------------------------------------------------------
    template <typename T> T inline saturate(T _value)
    {
        return clamp(_value, 0.0f, 1.0f);
    }

    //--------------------------------------------------------------------------------------
    template <typename T1, typename T2> T1 inline lerp(T1 _a, T1 _b, T2 _blend)
    {
        return _a + _blend * (_b - _a);
    }

    //--------------------------------------------------------------------------------------
    quaternion slerpShortestPath(quaternion _q0, quaternion _q1, float _blend);

    //--------------------------------------------------------------------------------------
    template <typename T> inline bool within(T _value, T _min, T _max)
    {
        return _value >= _min && _value <= _max;
    }

    //--------------------------------------------------------------------------------------
    float smoothdamp(float _currentValue, float _targetValue, float & _currentVelocity, float _duration, float _dt);
    float2 smoothdamp(float2 _currentValue, float2 _targetValue, float2 & _currentVelocity, float _duration, float _dt);
    float3 smoothdamp(float3 _currentValue, float3 _targetValue, float3 & _currentVelocity, float _duration, float _dt);
    float4 smoothdamp(float4 _currentValue, float4 _targetValue, float4 & _currentVelocity, float _duration, float _dt);

    //--------------------------------------------------------------------------------------
    inline core::u32 ctz(core::u32 value)
    {
        #ifdef _MSC_VER
        DWORD trailing_zero = 0;
        if (_BitScanForward(&trailing_zero, value))
            return trailing_zero;
        else
            return 32;
        #else
        if (0 != value)
            return __builtin_ctz(value);
        else
            return 32;
        #endif
    }

    //--------------------------------------------------------------------------------------
    inline core::u64 ctz(core::u64 value)
    {
        #ifdef _MSC_VER
        DWORD trailing_zero = 0;
        if (_BitScanForward64(&trailing_zero, value))
            return trailing_zero;
        else
            return 64;
        #else
        if (0 != value)
            return __builtin_ctzll(value);
        else
            return 64;
        #endif
    }

    //--------------------------------------------------------------------------------------
    inline core::u32 clz(core::u32 value)
    {
        #ifdef _MSC_VER
        DWORD leading_zero = 0;
        if (_BitScanReverse(&leading_zero, value))
            return 31 - leading_zero;
        else
            return 32;
        #else
        if (0 != value)
            return __builtin_clz(value);
        else
            return 32;
        #endif
    }

    //--------------------------------------------------------------------------------------
    inline core::u64 clz(core::u64 value)
    {
        #ifdef _MSC_VER
        DWORD leading_zero = 0;
        if (_BitScanReverse64(&leading_zero, value))
            return 63 - leading_zero;
        else
            return 64;
        #else
        if (0 != value)
            return __builtin_clzll(value);
        else
            return 64;
        #endif
    }

    //--------------------------------------------------------------------------------------
    template<typename T> inline T alignUp(T _adress, T _align)
    {
        VG_ASSERT((0 != _align) && !(_align & (_align - 1)));
        return (T(_adress + (_align - 1)) & ~(T)(_align - 1));
    }

    //--------------------------------------------------------------------------------------
    inline core::size_t Hash(const float & _this)
    {
        core::size_t hash = core::hash<float>()(_this) ^ 0xed558cc4ceb9fe1L;
        return hash;
    }

    //--------------------------------------------------------------------------------------
    inline core::size_t Hash(const core::float2 & _this)
    {
        core::size_t hash = core::hash<float>()(_this.x) ^ 0xc4ceb9fe1a85ec53L ^ core::hash<float>()(_this.y) ^ 0xed558cc4ceb9fe1L;
        return hash;
    }

    //--------------------------------------------------------------------------------------
    inline core::size_t Hash(const core::StorageFloat2 & _this)
    {
        core::size_t hash = core::hash<float>()(_this.x) ^ 0xc4ceb9fe1a85ec53L ^ core::hash<float>()(_this.y) ^ 0xed558cc4ceb9fe1L;
        return hash;
    }

    //--------------------------------------------------------------------------------------
    inline core::size_t Hash(const core::float3 & _this)
    {
        core::size_t hash = core::hash<float>()(_this.x) ^ 0xff51afd7ed558ccd ^core::hash<float>()(_this.y) ^ 0xc4ceb9fe1a85ec53L ^ core::hash<float>()(_this.z) ^ 0xed558cc4ceb9fe1L;
        return hash;
    }

    //--------------------------------------------------------------------------------------
    inline core::size_t Hash(const core::StorageFloat3 & _this)
    {
        core::size_t hash = core::hash<float>()(_this.x) ^ 0xff51afd7ed558ccd ^ core::hash<float>()(_this.y) ^ 0xc4ceb9fe1a85ec53L ^ core::hash<float>()(_this.z) ^ 0xed558cc4ceb9fe1L;
        return hash;
    }
    
    //--------------------------------------------------------------------------------------
    inline core::size_t Hash(const core::float4 & _this)
    {
        core::size_t hash = core::hash<float>()(_this.x) ^ core::hash<float>()(_this.y) ^ 0xff51afd7ed558ccd ^ core::hash<float>()(_this.z) ^ 0xc4ceb9fe1a85ec53L ^ core::hash<float>()(_this.w) ^ 0xed558cc4ceb9fe1L;
        return hash;
    }

    //--------------------------------------------------------------------------------------
    inline core::size_t Hash(const core::StorageFloat4 & _this)
    {
        core::size_t hash = core::hash<float>()(_this.x) ^ core::hash<float>()(_this.y) ^ 0xff51afd7ed558ccd ^ core::hash<float>()(_this.z) ^ 0xc4ceb9fe1a85ec53L ^ core::hash<float>()(_this.w) ^ 0xed558cc4ceb9fe1L;
        return hash;
    }
}

#if VG_ENABLE_INLINE
#include "Math.inl"
#endif