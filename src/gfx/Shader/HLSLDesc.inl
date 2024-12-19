namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    // HLSLDesc::Technique
    //--------------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------------
    VG_INLINE void HLSLDesc::Technique::setFlag(ShaderKey::Flags _flag, bool _enable)
    {
        VG_ASSERT(_flag < sizeof(Flags) << 3);
        if (_enable)
            flags |= 1 << _flag;
        else
            flags &= ~(1 << _flag);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void HLSLDesc::Technique::setFlags(ShaderKey::Flags _flag, core::uint _value)
    {
        // rem: this is really unsafe to do it from here without checking for overflow
        // If Technique knew the HLSLDesc or file index then it could perform the same checks as [Compute]ShaderKey::setFlags
        flags |= _value << _flag;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE bool HLSLDesc::Technique::operator == (const Technique & _other) const
    {
        return vs == _other.vs && hs == _other.hs && ds == _other.ds && gs == _other.gs && ps == _other.ps && _other.cs == cs && flags == _other.flags && 0 == name.compare(_other.name);
    }

    //--------------------------------------------------------------------------------------
    // HLSLDesc
    //--------------------------------------------------------------------------------------
    
    //--------------------------------------------------------------------------------------
    VG_INLINE bool HLSLDesc::isValidFlagIndex(core::uint _index) const
    {
        return m_flagDescs[_index].isInitialized();
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE core::u64 HLSLDesc::getCRC() const
    {
        return m_crc;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void HLSLDesc::setCRC(core::u64 _crc)
    {
        m_crc = _crc; 
    }
}