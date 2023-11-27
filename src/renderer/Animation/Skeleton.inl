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
}