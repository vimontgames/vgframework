namespace vg::core
{
    //--------------------------------------------------------------------------------------
    VG_INLINE bool Instance::isEnabled() const
    {
        return asBool(InstanceFlags::Enabled & getInstanceFlags());
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE bool Instance::isStatic() const
    {
        return asBool(InstanceFlags::Static & getInstanceFlags());
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void Instance::setLocalMatrix(const float4x4 & _local)
    {
        m_local = _local;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE const float4x4 & Instance::getLocalMatrix() const
    {
        return m_local;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void Instance::setGlobalMatrix(const float4x4 & _global)
    {
        if (Instance * parent = dynamic_cast<Instance *>(GetParent()))
            m_local = mul(_global, inverse(parent->getGlobalMatrix()));
        else
            m_local = _global;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE const float4x4 Instance::getGlobalMatrix() const
    {
        float4x4 global = m_local;
        if (Instance * parent = dynamic_cast<Instance *>(GetParent()))
            global = mul(global, parent->getGlobalMatrix());
        return global;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void Instance::setColor(const float4 & _color)
    {
        m_color = _color;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE const float4 & Instance::getColor() const
    {
        return m_color;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE Model * Instance::getModel(Lod _lod) const
    {
        VG_ASSERT(isEnumValue(_lod));
        if (asInteger(_lod) < m_models.size())
            return m_models[asInteger(_lod)];
        else
            return nullptr;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void Instance::setInstanceFlags(InstanceFlags _flags, bool _enabled)
    {
        if (_enabled)
            m_flags |= _flags;
        else
            m_flags &= ~_flags;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE InstanceFlags Instance::getInstanceFlags() const
    {
        return m_flags;
    }
}