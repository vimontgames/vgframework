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
    VG_INLINE bool HLSLDesc::isValidFlagValue(core::uint _index, core::uint _value) const
    {
        const uint bits = _value ? (uint)log2(_value) + 1: 1;
        if (m_flagDescs[_index].m_bits < bits)
            return false;

        return true;
    }
}