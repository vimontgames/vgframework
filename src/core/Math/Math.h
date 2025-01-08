#pragma once

#include "core/Types/Types.h"
#include <intrin.h>
#include "StorageVector.h"

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
    inline float4x4 getMatrixWithoutScale(const float4x4 & _matrix)
    {
        float4x4 matrix = _matrix;
        matrix[0].xyz = matrix[0].xyz / length(_matrix[0].xyz);
        matrix[1].xyz = matrix[1].xyz / length(_matrix[1].xyz);
        matrix[2].xyz = matrix[2].xyz / length(_matrix[2].xyz);
        return matrix;
    }

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

    static constexpr u64 table[256] =
    {
        0x000FF1CEBAADDEAD, 0xCAFE0D15EA5EFEED, 0xBAADF00DDEADF00D, 0xF00DDEAD10CC000F, 0xBAADB16B00B5FACE, 0xDEAD000FDEADDEAD, 0xDEAD10CCCAFEBABE, 0xFACEBAADFEED10CC,
        0x00BAB10CBAD2FEE1, 0xCAFEDABBAD00AAAD, 0xBAD22222FA112222, 0x2222FEE1DEAD00BA, 0xBAD20B00B135BAAA, 0xFEE100BADEADFA11, 0xFEE1DEADCAFED00D, 0xBAAABAD2AAADDEAD,
        0x1BADB002BADDDECA, 0xBAADFEEDBABE10CC, 0xBADDCAFEEA5ECAFE, 0xCAFEDECAFBAD1BAD, 0xBADD000FF1CEDEAD, 0xDECA1BAD0D15EA5E, 0xDECAFBADBAADF00D, 0xDEADBADD10CCFBAD,
        0xCEFAEDFEDEFEC000, 0xBAD2FEEDC0DEDEAD, 0xDEFEC8EDAD00C8ED, 0xC8EDC00010FFCEFA, 0xDEFE00BAB10CFEE1, 0xC000CEFADABBAD00, 0xC00010FFBAD22222, 0xFEE1DEFEDEAD10FF,
        0x8BADF00DB000D0D0, 0xBADDDEADBABEFBAD, 0xB000DEADBABEDEAD, 0xDEADD0D0CACA8BAD, 0xB0001BADB002DECA, 0xD0D08BADFEEDBABE, 0xD0D0CACABADDCAFE, 0xDECAB000FBADCACA,
        0xB105F00D0D06FACE, 0xDEFEDEADC0DE10FF, 0x0D06F00DC0DEF00D, 0xF00DFACEB00CB105, 0x0D06CEFAEDFEC000, 0xFACEB105FEEDC0DE, 0xFACEB00CDEFEC8ED, 0xC0000D0610FFB00C,
        0xB16B00B5CAFEFACE, 0xB000DEADDEADCACA, 0xCAFEBABEBABEBABE, 0xBABEFACEFEEDB16B, 0xCAFE8BADF00DD0D0, 0xFACEB16BDEADBABE, 0xFACEFEEDB000DEAD, 0xD0D0CAFECACAFEED,
        0x0B00B135CAFEBAAA, 0x0D06DEADFA11B00C, 0xCAFED00DC0DED00D, 0xD00DBAAAAAAD0B00, 0xCAFEB105F00DFACE, 0xBAAA0B00DEADC0DE, 0xBAAAAAAD0D06F00D, 0xFACECAFEB00CAAAD,
        0x000FF1CEBAADDEAD, 0xCAFE0D15EA5EFEED, 0xBAADF00DDEADF00D, 0xF00DDEAD10CC000F, 0xBAADB16B00B5FACE, 0xDEAD000FDEADDEAD, 0xDEAD10CCCAFEBABE, 0xFACEBAADFEED10CC,
        0x00BAB10CBAD2FEE1, 0xCAFEDABBAD00AAAD, 0xBAD22222FA112222, 0x2222FEE1DEAD00BA, 0xBAD20B00B135BAAA, 0xFEE100BADEADFA11, 0xFEE1DEADCAFED00D, 0xBAAABAD2AAADDEAD,
        0x1BADB002BADDDECA, 0xBAADFEEDBABE10CC, 0xBADDCAFEEA5ECAFE, 0xCAFEDECAFBAD1BAD, 0xBADD000FF1CEDEAD, 0xDECA1BAD0D15EA5E, 0xDECAFBADBAADF00D, 0xDEADBADD10CCFBAD,
        0xCEFAEDFEDEFEC000, 0xBAD2FEEDC0DEDEAD, 0xDEFEC8EDAD00C8ED, 0xC8EDC00010FFCEFA, 0xDEFE00BAB10CFEE1, 0xC000CEFADABBAD00, 0xC00010FFBAD22222, 0xFEE1DEFEDEAD10FF,
        0x8BADF00DB000D0D0, 0xBADDDEADBABEFBAD, 0xB000DEADBABEDEAD, 0xDEADD0D0CACA8BAD, 0xB0001BADB002DECA, 0xD0D08BADFEEDBABE, 0xD0D0CACABADDCAFE, 0xDECAB000FBADCACA,
        0xB105F00D0D06FACE, 0xDEFEDEADC0DE10FF, 0x0D06F00DC0DEF00D, 0xF00DFACEB00CB105, 0x0D06CEFAEDFEC000, 0xFACEB105FEEDC0DE, 0xFACEB00CDEFEC8ED, 0xC0000D0610FFB00C,
        0xB16B00B5CAFEFACE, 0xB000DEADDEADCACA, 0xCAFEBABEBABEBABE, 0xBABEFACEFEEDB16B, 0xCAFE8BADF00DD0D0, 0xFACEB16BDEADBABE, 0xFACEFEEDB000DEAD, 0xD0D0CAFECACAFEED,
        0x0B00B135CAFEBAAA, 0x0D06DEADFA11B00C, 0xCAFED00DC0DED00D, 0xD00DBAAAAAAD0B00, 0xCAFEB105F00DFACE, 0xBAAA0B00DEADC0DE, 0xBAAAAAAD0D06F00D, 0xFACECAFEB00CAAAD,
        0x000FF1CEBAADDEAD, 0xCAFE0D15EA5EFEED, 0xBAADF00DDEADF00D, 0xF00DDEAD10CC000F, 0xBAADB16B00B5FACE, 0xDEAD000FDEADDEAD, 0xDEAD10CCCAFEBABE, 0xFACEBAADFEED10CC,
        0x00BAB10CBAD2FEE1, 0xCAFEDABBAD00AAAD, 0xBAD22222FA112222, 0x2222FEE1DEAD00BA, 0xBAD20B00B135BAAA, 0xFEE100BADEADFA11, 0xFEE1DEADCAFED00D, 0xBAAABAD2AAADDEAD,
        0x1BADB002BADDDECA, 0xBAADFEEDBABE10CC, 0xBADDCAFEEA5ECAFE, 0xCAFEDECAFBAD1BAD, 0xBADD000FF1CEDEAD, 0xDECA1BAD0D15EA5E, 0xDECAFBADBAADF00D, 0xDEADBADD10CCFBAD,
        0xCEFAEDFEDEFEC000, 0xBAD2FEEDC0DEDEAD, 0xDEFEC8EDAD00C8ED, 0xC8EDC00010FFCEFA, 0xDEFE00BAB10CFEE1, 0xC000CEFADABBAD00, 0xC00010FFBAD22222, 0xFEE1DEFEDEAD10FF,
        0x8BADF00DB000D0D0, 0xBADDDEADBABEFBAD, 0xB000DEADBABEDEAD, 0xDEADD0D0CACA8BAD, 0xB0001BADB002DECA, 0xD0D08BADFEEDBABE, 0xD0D0CACABADDCAFE, 0xDECAB000FBADCACA,
        0xB105F00D0D06FACE, 0xDEFEDEADC0DE10FF, 0x0D06F00DC0DEF00D, 0xF00DFACEB00CB105, 0x0D06CEFAEDFEC000, 0xFACEB105FEEDC0DE, 0xFACEB00CDEFEC8ED, 0xC0000D0610FFB00C,
        0xB16B00B5CAFEFACE, 0xB000DEADDEADCACA, 0xCAFEBABEBABEBABE, 0xBABEFACEFEEDB16B, 0xCAFE8BADF00DD0D0, 0xFACEB16BDEADBABE, 0xFACEFEEDB000DEAD, 0xD0D0CAFECACAFEED,
        0x0B00B135CAFEBAAA, 0x0D06DEADFA11B00C, 0xCAFED00DC0DED00D, 0xD00DBAAAAAAD0B00, 0xCAFEB105F00DFACE, 0xBAAA0B00DEADC0DE, 0xBAAAAAAD0D06F00D, 0xFACECAFEB00CAAAD,
        0x000FF1CEBAADDEAD, 0xCAFE0D15EA5EFEED, 0xBAADF00DDEADF00D, 0xF00DDEAD10CC000F, 0xBAADB16B00B5FACE, 0xDEAD000FDEADDEAD, 0xDEAD10CCCAFEBABE, 0xFACEBAADFEED10CC,
        0x00BAB10CBAD2FEE1, 0xCAFEDABBAD00AAAD, 0xBAD22222FA112222, 0x2222FEE1DEAD00BA, 0xBAD20B00B135BAAA, 0xFEE100BADEADFA11, 0xFEE1DEADCAFED00D, 0xBAAABAD2AAADDEAD,
        0x1BADB002BADDDECA, 0xBAADFEEDBABE10CC, 0xBADDCAFEEA5ECAFE, 0xCAFEDECAFBAD1BAD, 0xBADD000FF1CEDEAD, 0xDECA1BAD0D15EA5E, 0xDECAFBADBAADF00D, 0xDEADBADD10CCFBAD,
        0xCEFAEDFEDEFEC000, 0xBAD2FEEDC0DEDEAD, 0xDEFEC8EDAD00C8ED, 0xC8EDC00010FFCEFA, 0xDEFE00BAB10CFEE1, 0xC000CEFADABBAD00, 0xC00010FFBAD22222, 0xFEE1DEFEDEAD10FF,
        0x8BADF00DB000D0D0, 0xBADDDEADBABEFBAD, 0xB000DEADBABEDEAD, 0xDEADD0D0CACA8BAD, 0xB0001BADB002DECA, 0xD0D08BADFEEDBABE, 0xD0D0CACABADDCAFE, 0xDECAB000FBADCACA,
        0xB105F00D0D06FACE, 0xDEFEDEADC0DE10FF, 0x0D06F00DC0DEF00D, 0xF00DFACEB00CB105, 0x0D06CEFAEDFEC000, 0xFACEB105FEEDC0DE, 0xFACEB00CDEFEC8ED, 0xC0000D0610FFB00C,
        0xB16B00B5CAFEFACE, 0xB000DEADDEADCACA, 0xCAFEBABEBABEBABE, 0xBABEFACEFEEDB16B, 0xCAFE8BADF00DD0D0, 0xFACEB16BDEADBABE, 0xFACEFEEDB000DEAD, 0xD0D0CAFECACAFEED,
        0x0B00B135CAFEBAAA, 0xCAFEDEADFA11AAAD, 0xCAFED00DFA11D00D, 0xD00DBAAAAAAD0B00, 0xCAFE0B00B135BAAA, 0xBAAA0B00DEADFA11, 0xBAAAAAADCAFED00D, 0xBAAACAFEAAADAAAD,
    };

    //--------------------------------------------------------------------------------------
    constexpr u64 computeCRC64(const char * _string, const size_t _length = 0)
    {
        u64 crc = 0;
        size_t len = _length;

        if (!len)
            len = strlen(_string);

        crc = crc ^ 0xFFFFFFFF;
        for (size_t i = 0; i < len; i++)
            crc = table[_string[i] ^ (crc & 0xFF)] ^ (crc >> 8UL);
        crc = crc ^ 0xFFFFFFFF;

        return crc;
    }

    #define crc64(str) std::integral_constant<u64, computeCRC64(str, sizeof(str)-1)>::value

    template <typename T> inline T ctz(T _value);
    template <typename T> inline T clz(T _value);

    //--------------------------------------------------------------------------------------
    template<> inline core::u32 ctz(core::u32 value)
    {
        DWORD trailing_zero = 0;

        if (_BitScanForward(&trailing_zero, value))
            return trailing_zero;
        else
            return 32;
    }

    //--------------------------------------------------------------------------------------
    template<> inline core::u64 ctz(core::u64 value)
    {
        DWORD trailing_zero = 0;

        if (_BitScanForward64(&trailing_zero, value))
            return trailing_zero;
        else
            return 64;
    }    

    //--------------------------------------------------------------------------------------
    template<> inline core::u32 clz(core::u32 value)
    {
        DWORD leading_zero = 0;
        if (_BitScanReverse(&leading_zero, value))
            return 31 - leading_zero;
        else
            return 32;
    }

    //--------------------------------------------------------------------------------------
    template<> inline core::u64 clz(core::u64 value)
    {
        DWORD leading_zero = 0;
        if (_BitScanReverse64(&leading_zero, value))
            return 63 - leading_zero;
        else
            return 63;
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