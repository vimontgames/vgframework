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
    bool MeshInstance::setSkinFlag(SkinFlags _flag)
    {
        return 0 == (_flag & m_skinFlags.fetch_or(_flag)); 
    }

    //--------------------------------------------------------------------------------------
    // Returns true if the flag was just removed
        //--------------------------------------------------------------------------------------
    bool MeshInstance::clearSkinFlag(SkinFlags _flag)
    {
        return 0 != (_flag & m_skinFlags.fetch_and(~_flag)); 
    }
}