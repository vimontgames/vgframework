namespace vg::core
{
    //--------------------------------------------------------------------------------------
    VG_INLINE void AABB::grow(const core::float3 & _point)
    {
        m_min = min(m_min, _point);
        m_max = max(m_max, _point);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void AABB::grow(const core::AABB & _aabb)
    {
        m_min = min(m_min, _aabb.m_min);
        m_max = max(m_max, _aabb.m_max);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE core::float3 AABB::center() const
    {
        return (m_min + m_max) * 0.5f;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE float AABB::radius() const
    {
        auto size = m_max - m_min;
        return max(size.x, max(size.y, size.z)) * 0.5f;
    }
}