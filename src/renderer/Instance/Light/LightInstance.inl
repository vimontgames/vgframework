namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    VG_INLINE void LightInstance::setAmbient(const core::float3 & _ambient)
    {
        m_ambient = _ambient;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE const core::float3 & LightInstance::getAmbient() const
    {
        return m_ambient;
    }
}