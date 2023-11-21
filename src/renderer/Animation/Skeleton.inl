namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    VG_INLINE const core::vector<core::u32> & Skeleton::getBoneIndices() const
    {
        return m_boneIndices;
    }
    
    //--------------------------------------------------------------------------------------
    VG_INLINE const core::vector<core::float4x4> & Skeleton::getBoneMatrices() const
    {
        return m_boneMatrices;
    }
}