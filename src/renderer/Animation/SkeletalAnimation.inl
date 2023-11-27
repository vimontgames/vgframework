namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    VG_INLINE float SkeletalAnimation::getTime() const
    { 
        return m_time; 
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE float SkeletalAnimation::getWeight() const
    {
        return m_weight;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE const AnimImporterData & SkeletalAnimation::getAnimationData() const
    {
        return m_animData;
    }
}