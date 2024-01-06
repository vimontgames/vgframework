namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    VG_INLINE const Skeleton * MeshInstance::getInstanceSkeleton() const
    {
        return m_instanceSkeleton;
    }

    //--------------------------------------------------------------------------------------
    // Returns true if the flag was just set
    //--------------------------------------------------------------------------------------
    VG_INLINE bool MeshInstance::setSkinFlag(SkinFlags _flag)
    {
        return 0 == (_flag & m_skinFlags.fetch_or(_flag)); 
    }

    //--------------------------------------------------------------------------------------
    // Returns true if the flag was just removed
    //--------------------------------------------------------------------------------------
    VG_INLINE bool MeshInstance::clearSkinFlag(SkinFlags _flag)
    {
        return 0 != (_flag & m_skinFlags.fetch_and(~_flag)); 
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE const gfx::Buffer * MeshInstance::getSkinnedMeshBuffer() const
    {
        return m_skinnedMeshBuffer;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE const core::uint MeshInstance::getSkinnedMeshBufferOffset() const
    {
        return m_skinnedMeshBufferOffset;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void MeshInstance::setInstanceBLASes(gfx::BLASCollection * _BLASes)
    {
        if (_BLASes != m_instanceBLASes)
        {
            VG_SAFE_RELEASE_ASYNC(m_instanceBLASes);
            m_instanceBLASes = _BLASes;
            VG_SAFE_INCREASE_REFCOUNT(m_instanceBLASes);
        }
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE gfx::BLASCollection * MeshInstance::getInstanceBLASes() const
    {
        return m_instanceBLASes;
    }
}