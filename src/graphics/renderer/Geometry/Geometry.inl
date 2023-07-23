namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    VG_INLINE void Geometry::setAABB(const AABB & _aabb)
    {
        m_aabb = _aabb;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE const AABB & Geometry::getAABB() const
    {
        return m_aabb;
    }
}