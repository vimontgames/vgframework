namespace vg::core
{
    //--------------------------------------------------------------------------------------
    VG_INLINE const vector<GameObject *> & GameObject::getChildren() const 
    { 
        return m_children; 
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE UpdateFlags GameObject::getUpdateFlags() const
    {
        return m_update;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void GameObject::setUpdateFlags(UpdateFlags _flags, bool _enabled)
    {
        if (_enabled)
            m_update |= _flags;
        else
            (std::underlying_type<UpdateFlags>::type &)m_update &= ~(std::underlying_type<UpdateFlags>::type &)_flags;
    }
}