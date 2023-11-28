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
    bool MeshInstance::setRuntimeFlag(RuntimeFlags _flag)
    {
        return 0 == (_flag & m_runtimeFlags.fetch_or(_flag)); 
    }

    //--------------------------------------------------------------------------------------
    // Returns true if the flag was just removed
        //--------------------------------------------------------------------------------------
    bool MeshInstance::clearRuntimeFlag(RuntimeFlags _flag)
    {
        return 0 != (_flag & m_runtimeFlags.fetch_and(~_flag)); 
    }
}