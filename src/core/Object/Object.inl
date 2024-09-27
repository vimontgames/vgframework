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
            m_objectFlags &= ~_flags;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void Object::setParent(IObject * _parent)
    {
        m_parent = (Object *)_parent;

        #ifndef VG_FINAL
        if (_parent)
            m_parentNameDbg = (core::string)"(" + m_parent->GetClassName() + (core::string)") " + m_parent->GetName();
        else
            m_parentNameDbg = "(null)";
        #endif
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE IObject * Object::getParent() const
    {
        return (IObject *)m_parent;
    }
}