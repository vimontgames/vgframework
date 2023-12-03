namespace vg::core
{
    //--------------------------------------------------------------------------------------
    VG_INLINE void Instance::setWorldMatrix(const float4x4 & _world)
    {
        m_world = _world;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE const float4x4 & Instance::getWorldMatrix() const
    {
        return m_world;
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
    VG_INLINE void Instance::setFlags(Flags _flags, bool _enabled)
    {
        if (_enabled)
            m_flags |= _flags;
        else
            (std::underlying_type<Flags>::type &)m_flags &= ~(std::underlying_type<Flags>::type)_flags;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE IInstance::Flags Instance::getFlags() const
    {
        return m_flags;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void Instance::setRuntimeFlags(RuntimeFlags _flags, bool _enabled)
    {
        if (_enabled)
            m_runtimeFlags |= _flags;
        else
            (std::underlying_type<RuntimeFlags>::type&)m_runtimeFlags &= ~(std::underlying_type<RuntimeFlags>::type)_flags;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE IInstance::RuntimeFlags Instance::getRuntimeFlags() const
    {
        return m_runtimeFlags;
    }
}