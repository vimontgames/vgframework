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

    //--------------------------------------------------------------------------------------
    VG_INLINE GameObject * Component::getGameObject() const
    {
        const auto * parent = getParent();
        VG_ASSERT(nullptr != parent, "Could not find parent GameObject of Component \"%s\"", getClassName());
        return (GameObject *)parent;
    }
}