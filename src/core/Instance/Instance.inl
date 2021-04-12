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
}