namespace vg::engine
{
    //--------------------------------------------------------------------------------------
    VG_INLINE const core::Time & Engine::getTime() const
    {
        return m_time;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE bool Engine::isPlaying() const
    {
        return m_isPlaying;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE bool Engine::isPaused() const
    {
        return m_isPaused;
    }
}