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
        bool setNames(core::vector<core::string> && _names);
        bool setBitValue(core::uint _index, bool _value);

        VG_INLINE core::uint getBitCount() const;
        VG_INLINE const core::vector<core::string> & getNames() const;
        VG_INLINE bool getBitValue(core::uint _index) const;

        core::string toString(bool _leadingZeroes = false) const;
        bool fromString(const core::string & _value, core::uint _bitCount);

        bool toBuffer(io::Buffer & _buffer);
        bool fromBuffer(io::Buffer & _buffer);
        
    private:
        VG_INLINE core::uint getNumBitsPerItem() const;

    private:
        core::uint m_bitCount = 0;
        core::vector<T> m_bits;
        core::vector<core::string> m_names;
    };
}

#if VG_ENABLE_INLINE
#include "BitMask.inl" 
#endif