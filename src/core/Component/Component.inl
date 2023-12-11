namespace vg::core
{
    //--------------------------------------------------------------------------------------
    VG_INLINE IComponent::Flags Component::getFlags() const
    {
        return m_flags;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void Component::setFlags(Flags _flags, bool _enabled)
    {
        if (_enabled)
            m_flags |= _flags;
        else
            (std::underlying_type<Flags>::type&)m_flags &= ~(std::underlying_type<Flags>::type&)_flags;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE UpdateFlags Component::getUpdateFlags() const
    {
        return m_update;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void Component::setUpdateFlags(UpdateFlags _flags, bool _enabled)
    {
        if (_enabled)
            m_update |= _flags;
        else
            (std::underlying_type<UpdateFlags>::type&)m_update &= ~(std::underlying_type<UpdateFlags>::type&)_flags;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE GameObject * Component::getGameObject() const
    {
        const auto * parent = getParent();
        VG_ASSERT(nullptr != parent, "Could not find parent GameObject of Component \"%s\"", getClassName());
        return (GameObject *)parent;
    }
}