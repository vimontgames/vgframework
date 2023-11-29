#pragma once

#include "core/Types/Types.h"

namespace vg::core
{
    const float PI = 3.14159265359f;

    const float MIN_FLOAT = FLT_MIN;
    const float MAX_FLOAT = FLT_MAX;

    //--------------------------------------------------------------------------------------
    inline float4x4 TRS(const float3 & _translation, const quaternion _rotation, const float3 _scale)
    {
        quaternion q = _rotation;

        float xx = q.x * q.x, xy = q.x * q.y, xz = q.x * q.z, xw = q.x * q.w;
        float yy = q.y * q.y, yz = q.y * q.z, yw = q.y * q.w;
        float zz = q.z * q.z, zw = q.z * q.w;
        float sx = 2.0f * _scale.x, sy = 2.0f * _scale.y, sz = 2.0f * _scale.z;

        return float4x4(
            sx * (-yy - zz + 0.5f), sy * (-zw + xy), sz * (+xz + yw), _translation.x,
            sx * (+xy + zw), sy * (-xx - zz + 0.5f), sz * (-xw + yz), _translation.y,
            sx * (-yw + xz), sy * (+xw + yz), sz * (-xx - yy + 0.5f), _translation.z,
            0, 0, 0, 1
        );
    }

	//--------------------------------------------------------------------------------------
	template <typename T> T min(const T & _left, const T & _right)
	{
		return _left < _right ? _left : _right;
	}

	//--------------------------------------------------------------------------------------
	template <typename T> T max(const T & _left, const T & _right)
	{
		return _left > _right ? _left : _right;
	}

    //--------------------------------------------------------------------------------------
    template <class T> inline T clamp(T _value, T _min, T _max)
    {
        return (_value < _min) ? _min : ((_value > _max) ? _max : _value);
    }

    //--------------------------------------------------------------------------------------
    template <typename T> inline bool within(T _value, T _min, T _max)
    {
        return _value >= _min && _value <= _max;
    }

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

    template <typename T> inline T countl_zero(T _value);

    //--------------------------------------------------------------------------------------
    template<> inline u32 countl_zero(u32 _value)
    {
        return _lzcnt_u32(_value);
    }

    //--------------------------------------------------------------------------------------
    template<> inline u64 countl_zero(u64 _value)
    {
        return _lzcnt_u64(_value);
    }

    //--------------------------------------------------------------------------------------
    template<typename T> inline T alignUp(T _adress, T _align)
    {
        VG_ASSERT((0 != _align) && !(_align & (_align - 1)));
        return (T(_adress + (_align - 1)) & ~(T)(_align - 1));
    }

    //--------------------------------------------------------------------------------------
    inline core::size_t Hash(const core::float2 & _this)
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
    inline core::size_t Hash(const core::float4 & _this)
    {
        core::size_t hash = core::hash<float>()(_this.x) ^ core::hash<float>()(_this.y) ^ 0xff51afd7ed558ccd ^ core::hash<float>()(_this.z) ^ 0xc4ceb9fe1a85ec53L ^ core::hash<float>()(_this.w) ^ 0xed558cc4ceb9fe1L;
        return hash;
    }
}