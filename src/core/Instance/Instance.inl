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
    VG_INLINE Model * Instance::getModel(Lod _lod) const
    {
        VG_ASSERT(isEnumValue(_lod));

        return m_models[asInteger(_lod)];
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