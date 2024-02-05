namespace vg::core
{
    //--------------------------------------------------------------------------------------
    VG_INLINE core::uint BitMask::getNumBitsPerItem() const
    {
        return (sizeof(T) << 3);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE core::uint BitMask::getBitCount() const
    {
        return m_bitCount;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE bool BitMask::getBitValue(core::uint _index) const
    {
        auto itemIndex = _index / getNumBitsPerItem();
        auto localBitIndex = _index % getNumBitsPerItem();

        VG_ASSERT(itemIndex < m_bits.size());
        VG_ASSERT(localBitIndex < getNumBitsPerItem());

        return m_bits[itemIndex] & (T(1) << T(localBitIndex));
    }
}