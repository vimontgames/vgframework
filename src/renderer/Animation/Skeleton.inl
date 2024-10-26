namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    VG_INLINE core::uint Skeleton::getBoneCount() const
    {
        VG_ASSERT(m_boneIndices.size() == m_boneMatrices.size());
        return (core::uint)m_boneIndices.size();
    }
    
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

    //--------------------------------------------------------------------------------------
    VG_INLINE const core::vector<MeshImporterNode> & Skeleton::getNodes() const
    {
        return m_nodes;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE core::vector<MeshImporterNode> & Skeleton::getNodes()
    {
        return m_nodes;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE float Skeleton::getScale() const
    {
        return m_scale;
    }
}