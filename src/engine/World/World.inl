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

    //--------------------------------------------------------------------------------------
    VG_INLINE void World::setTimeScale(float _timeScale)
    {
        m_timeScale = _timeScale;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE float World::getTimeScale(bool _ignorePause) const
    {
        return (!_ignorePause && isPaused()) ? 0.0f : m_timeScale;
    }
}