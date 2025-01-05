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
    VG_INLINE gfx::RenderJobsPolicy RendererOptions::getRenderJobsPolicy() const
    {
        return m_renderJobsPolicy;
    }
}