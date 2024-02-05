#pragma once

#include "core/Types/Types.h"

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

        core::string toString(bool _leadingZeroes = false) const;
        bool fromString(const core::string & _value, core::uint _bitCount);
        
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