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

    //--------------------------------------------------------------------------------------
    VG_INLINE void Object::setParent(IObject * _parent)
    {
        m_parent = (Object *)_parent;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE IObject * Object::getParent() const
    {
        return (IObject *)m_parent;
    }
}