namespace vg::engine
{
    //--------------------------------------------------------------------------------------
    VG_INLINE bool World::isPlaying() const
    {
        return m_isPlaying;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE bool World::isPaused() const
    {
        return m_isPaused;
    }
}