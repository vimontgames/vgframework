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
        const Object * parent = (Object*)getParent();
        if (parent && asBool(ObjectRuntimeFlags::Instance & parent->getObjectRuntimeFlags()))
            m_local = mul(_global, inverse(((Instance *)parent)->getGlobalMatrix()));
        else
            m_local = _global;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE const float4x4 Instance::getGlobalMatrix() const
    {
        float4x4 global = m_local;
        const Object * parent = (Object *)getParent();
        if (parent && asBool(ObjectRuntimeFlags::Instance & parent->getObjectRuntimeFlags()))
            global = mul(global, ((Instance*)parent)->getGlobalMatrix());
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

    //--------------------------------------------------------------------------------------
    VG_INLINE void Instance::setInstanceRuntimeFlags(InstanceRuntimeFlags _flags, bool _enabled)
    {
        if (_enabled)
            m_runtimeFlags |= _flags;
        else
            m_runtimeFlags &= ~_flags;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE InstanceRuntimeFlags Instance::getInstanceRuntimeFlags() const
    {
        return m_runtimeFlags;
    }
}