#include "core/Precomp.h"
#include "BitMask.h"

#if !VG_ENABLE_INLINE
#include "BitMask.inl"
#endif

#include "core/string/string.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    BitMask::BitMask(uint _bitCount, bool _default)
    {
        setBitCount(_bitCount, _default);
    }

    //--------------------------------------------------------------------------------------
    bool BitMask::setBitCount(core::uint _count, bool _default)
    {
        auto itemCount = (_count + getNumBitsPerItem()-1) / getNumBitsPerItem();
        auto previousBitCount = m_bitCount;

        if (m_bitCount != _count)
        {
            if (m_bits.size() != itemCount)
                m_bits.resize(itemCount);

            m_bitCount = _count;

            for (uint i = previousBitCount; i < m_bitCount; ++i)
                setBitValue(i, _default);

            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    //bool BitMask::setNames(const core::vector<core::string> & _names)
    //{
    //    VG_ASSERT(_names.size() == m_bitCount);
    //    m_names = _names;
    //    return true; // Return 'false' if unchanged?
    //}

    //--------------------------------------------------------------------------------------
    bool BitMask::setBitValue(core::uint _index, bool _value)
    {
        auto itemIndex = _index / getNumBitsPerItem();
        auto localBitIndex = _index % getNumBitsPerItem();

        VG_ASSERT(itemIndex < m_bits.size());
        VG_ASSERT(localBitIndex < getNumBitsPerItem());

        auto & item = m_bits[itemIndex & (1 << localBitIndex)];
        
        if ((0 != (item & (T(1)<<T(localBitIndex))) != _value))
        {
            if (_value)
                item |= T(1)<<T(localBitIndex);
            else
                item &= ~(T(1)<<T(localBitIndex));

            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    string BitMask::toString(bool _leadingZeroes) const
    {
        if (m_bitCount != 0)
        {
            string s;
            for (uint i = 0; i < m_bits.size(); ++i)
            {
                if (i + 1 != m_bits.size() || _leadingZeroes)
                {
                    switch (sizeof(T))
                    {
                        default:
                            VG_ASSERT(false, "sizeof(T) == %u is not implemented", sizeof(T));
                            break;

                        case 4:
                            s += fmt::sprintf("%08X", m_bits[i]);
                            break;

                        case 8:
                            s += fmt::sprintf("%016X", m_bits[i]);
                            break;
                    }
                }
                else
                {
                    s = fmt::sprintf("%X", m_bits[i]) + s;
                }

            }
            s = "0x" + s;
            return s;
        }
        else
        {
            return "0x0";
        }
    }

    //--------------------------------------------------------------------------------------
    bool BitMask::fromString(const core::string & _value, core::uint _bitCount)
    {
        if (!_value._Starts_with("0x"))
            return false;

        uint itemCount = (_bitCount + getNumBitsPerItem() - 1) / getNumBitsPerItem();
        uint charCount = sizeof(T) << 1;
        core::vector<T> bits;

        setBitCount(_bitCount);

        for (uint i = 0; i < itemCount; ++i)
        {
            uint index = (itemCount - i - 1);
            string temp = _value.substr(2 + charCount * index, charCount);
            u64 value = strtoull(temp.c_str(), nullptr, 16);
            
            for (uint j = 0; j < getNumBitsPerItem(); ++j)
            {
                if (index < _bitCount)
                {
                    if (value & (T(1) << T(j)))
                        setBitValue(index * getNumBitsPerItem() + j, true);
                    else
                        setBitValue(index * getNumBitsPerItem() + j, false);
                }
            }
        }

        return true; 
    }

    //--------------------------------------------------------------------------------------
    bool BitMask::toBuffer(io::Buffer & _buffer)
    {
        bool result = _buffer.write((u32)m_bitCount);
             result |= _buffer.write((u32)m_bits.size());

        for (uint i = 0; i < m_bits.size(); ++i)
            result |= _buffer.write((u64)m_bits[i]);

        return result;
    }

    //--------------------------------------------------------------------------------------
    bool BitMask::fromBuffer(io::Buffer & _buffer)
    {
        u32 bitCount = 0;
        bool result  = _buffer.read(&bitCount);

        u32 size = 0;
        result |= _buffer.read(&size);

        if (result)
        {
            setBitCount(bitCount);
            for (uint i = 0; i < size; ++i)
                result |= _buffer.read(&m_bits[i]);
        }

        return result;
    }

    //--------------------------------------------------------------------------------------
    bool BitMask::operator == (const BitMask & _other) const
    {
        if (m_bitCount != _other.m_bitCount)
            return false;

        const auto itemCount = (m_bitCount + getNumBitsPerItem() - 1) / getNumBitsPerItem();
        for (uint i = 0; i < itemCount; ++i)
        {
            if (m_bits[i] != _other.m_bits[i])
                return false;
        }

        return true;
    }
}