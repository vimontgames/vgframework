namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
// Returns true if the flag was just set
//--------------------------------------------------------------------------------------
    VG_INLINE bool GraphicInstance::setAtomicFlags(AtomicFlags _flag) const
    {
        return 0 == (_flag & m_atomicFlags.fetch_or(_flag));
    }

    //--------------------------------------------------------------------------------------
    // Returns true if the flag was just removed
    //--------------------------------------------------------------------------------------
    VG_INLINE bool GraphicInstance::removeAtomicFlags(AtomicFlags _flag)
    {
        return 0 != (_flag & m_atomicFlags.fetch_and(~_flag));
    }

    //--------------------------------------------------------------------------------------
    // Offset in Buffer 'RESERVEDSLOT_BUFSRV_INSTANCEDATA' with Instance and its material data
    // Filled once during InstanceDataUpdatePass. 
    //--------------------------------------------------------------------------------------
    VG_INLINE void GraphicInstance::setGPUInstanceDataOffset(core::uint _offset)
    {
        m_gpuInstanceDataHandle = _offset;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE core::uint GraphicInstance::getGPUInstanceDataOffset() const
    {
        return m_gpuInstanceDataHandle;
    }  

    //--------------------------------------------------------------------------------------
    VG_INLINE const core::BitMask & GraphicInstance::getBatchMask() const
    {
        return m_batchMask;
    }
}