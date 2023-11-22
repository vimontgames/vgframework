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
    VG_INLINE void Instance::setFlags(Flags flags, bool enabled)
    {
        if (enabled)
            m_flags |= flags;
        else
            (u32&)m_flags &= ~(u32)flags;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE IInstance::Flags Instance::getFlags() const
    {
        return m_flags;
    }
}