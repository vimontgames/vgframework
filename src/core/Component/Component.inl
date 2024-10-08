namespace vg::core
{
    //--------------------------------------------------------------------------------------
    VG_INLINE bool Component::isEnabled() const
    {
        return asBool(ComponentFlags::Enabled & getComponentFlags());
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE ComponentFlags Component::getComponentFlags() const
    {
        return m_flags;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void Component::setComponentFlags(ComponentFlags _flags, bool _enabled)
    {
        if (_enabled)
            m_flags |= _flags;
        else
            m_flags &= ~_flags;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE UpdateFlags Component::getUpdateFlags() const
    {
        return m_update;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void Component::enableUpdateFlags(UpdateFlags _flags, bool _enabled)
    {
        if (_enabled)
            m_update |= _flags;
        else
            (std::underlying_type<UpdateFlags>::type&)m_update &= ~(std::underlying_type<UpdateFlags>::type&)_flags;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE GameObject * Component::getGameObject() const
    {
        const auto * parent = GetParent();
        VG_ASSERT(nullptr != parent, "Could not find parent GameObject of Component \"%s\"", GetClassName());
        return (GameObject *)parent;
    }
}