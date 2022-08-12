namespace vg::core
{
    //--------------------------------------------------------------------------------------
    VG_INLINE IComponent::Flags Component::getFlags() const
    {
        return m_flags;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void Component::setFlags(Flags flags, bool enabled)
    {
        if (enabled)
            m_flags |= flags;
        else
            (u32&)m_flags &= ~(u32)flags;
    }
}