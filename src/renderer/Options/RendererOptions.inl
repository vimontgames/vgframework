namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    VG_INLINE bool RendererOptions::isRenderJobsEnabled() const
    {
        return m_renderJobs;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE bool RendererOptions::isForcedRenderJobsCount() const
    {
        return m_forceRenderJobsCount;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE bool RendererOptions::isRenderJobsUsingOnlyMainThread() const
    {
        return m_renderJobsOnMainThreadOnly;
    }
}