namespace vg::engine
{
    //--------------------------------------------------------------------------------------
    VG_INLINE SurfaceType DefaultMaterialData::getSurfaceType() const
    {
        return m_surfaceType;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE UVSource DefaultMaterialData::getUVSource() const
    {
        return m_UVSource; 
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE core::uint DefaultMaterialData::getFrameCount() const
    {
        if (getUVSource() == UVSource::FlipBook)
            return core::uint(ceil((float)m_tiling.x) * ceil((float)m_tiling.y) - 1);
        else
            return 0;
    }
}