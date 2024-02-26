namespace vg::core
{
    //--------------------------------------------------------------------------------------
    VG_INLINE ObjectFlags Object::getObjectFlags() const
    {
        return m_objectFlags;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void Object::setObjectFlags(ObjectFlags _flags, bool _enabled)
    {
        if (_enabled)
            m_objectFlags |= _flags;
        else
            (std::underlying_type<ObjectFlags>::type&)m_objectFlags &= ~(std::underlying_type<ObjectFlags>::type)_flags;
    }
}