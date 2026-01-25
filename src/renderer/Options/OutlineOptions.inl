namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    VG_INLINE void OutlineOptions::setZPassOutlineColor(const core::float4 & _color)
    {
        m_zPassOutlineColor = _color;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE const core::float4 & OutlineOptions::getZPassOutlineColor() const
    {
        return m_zPassOutlineColor;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void OutlineOptions::setZFailOutlineColor(const core::float4 & _color)
    {
        m_zFailOutlineColor = _color;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE const core::float4 & OutlineOptions::getZFailOutlineColor() const
    {
        return m_zFailOutlineColor;
    }
}