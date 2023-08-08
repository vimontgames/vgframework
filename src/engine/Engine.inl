namespace vg::engine
{
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

    //--------------------------------------------------------------------------------------
    VG_INLINE void Engine::play()
    {
        m_isPlaying = true;
        m_isPaused = false;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void Engine::pause()
    {
        VG_ASSERT(m_isPlaying);
        m_isPaused = true;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void Engine::stop()
    {
        m_isPlaying = false;
        m_isPaused = false;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE const Time & Engine::getTime() const
    {
        return m_time;
    }
}