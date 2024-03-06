namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    VG_INLINE void Geometry::setAABB(const core::AABB & _aabb)
    {
        m_aabb = _aabb;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE const core::AABB & Geometry::getAABB() const
    {
        return m_aabb;
    }
}