#pragma once

#include "core/Types/Types.h"
#include "core/File/Buffer.h"

namespace vg::core
{
    class BitMask 
    {
    public:
        using T = core::u64;

        BitMask(core::uint _bitCount = 0, bool _default = false);

        bool setBitCount(core::uint _count, bool _default = false);
        bool setBitValue(core::uint _index, bool _value);

        VG_INLINE core::uint getBitCount() const;
        VG_INLINE bool getBitValue(core::uint _index) const;
        VG_INLINE T getValue(core::uint _pageIndex = 0) const;

        core::string toString(bool _leadingZeroes = false) const;
        bool fromString(const core::string & _value, core::uint _bitCount);

        bool toBuffer(io::Buffer & _buffer);
        bool fromBuffer(io::Buffer & _buffer);

        bool operator == (const BitMask & _other) const;
        
    private:
        VG_INLINE core::uint getNumBitsPerItem() const;

    private:
        core::uint m_bitCount = 0;
        core::vector<T> m_bits;
    };
}

#if VG_ENABLE_INLINE
#include "BitMask.inl" 
#endif