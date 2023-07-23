namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    VG_INLINE void AABB::grow(const core::float3 & _point)
    {
        m_min = min(m_min, _point);
        m_max = max(m_max, _point);
    }
}