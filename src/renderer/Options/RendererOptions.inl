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

    //--------------------------------------------------------------------------------------
    VG_INLINE core::uint RendererOptions::getMaxRenderTotalBufferSize() const
    {
        return m_renderJobsTotalBufferSizeInMB * 1024 * 1024;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE core::uint RendererOptions::getMaxRenderMinBufferSize() const
    {
        return m_renderJobsWorkerMinBufferSizeInMB * 1024 * 1024;
    }
}