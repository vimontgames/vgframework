namespace vg::core
{
    //--------------------------------------------------------------------------------------
    VG_INLINE bool Instance::isLocalEnabled() const
    {
        return asBool(InstanceFlags::Enabled & getInstanceFlags());
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE bool Instance::isEnabledInHierarchy() const
    {
        return asBool(InstanceRuntimeFlags::Active & getInstanceRuntimeFlags());
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE bool Instance::isStatic() const
    {
        return asBool(InstanceFlags::Static & getInstanceFlags());
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void Instance::setLocalMatrix(const float4x4 & _local)
    {
        if (any(m_local != _local))
        {
            m_local = _local;

            OnLocalMatrixChanged(false, true);
        }
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE const float4x4 & Instance::getLocalMatrix() const
    {
        return m_local;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void Instance::setGlobalMatrix(const float4x4 & _global)
    {
        if (any(_global != m_global))
        {
            m_global = _global;

            const Object * parent = (Object *)getParent();
            if (parent && asBool(ObjectRuntimeFlags::Instance & parent->getObjectRuntimeFlags()))
            {
                Instance * parentInstance = (Instance *)parent;
                m_local = mul(_global, inverse(parentInstance->getGlobalMatrix()));
            }
            else
            {
                m_local = m_global;
            }

            OnLocalMatrixChanged(false, true);
        }
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE const float4x4 Instance::getGlobalMatrix() const
    {
        return m_global;
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
    VG_INLINE bool Instance::hasAnyModel() const
    {
        for (uint i = 0; i < m_models.size(); ++i)
        {
            if (nullptr != m_models[i])
                return true;
        }
        return false;
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