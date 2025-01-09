#pragma once

namespace vg::gfx
{
    namespace base
    {
        //--------------------------------------------------------------------------------------
        VG_INLINE double Device::getGpuFrameTime() const
        {
            return m_gpuFrameTime;
        }

        //--------------------------------------------------------------------------------------
        VG_INLINE double Device::getGpuWaitTime() const
        {
            return m_gpuWaitTime;
        } 
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE core::uint Device::getMaxRenderJobCount() const
    {
        VG_ASSERT(-1 != m_maxRenderJobCount);
        return m_maxRenderJobCount;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE gfx::RenderJobsPolicy Device::getRenderJobsPolicy() const
    {
        return m_renderJobsPolicy;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE core::uint Device::getMaxRenderTotalBufferSize() const
    {
        return m_renderJobsTotalBufferSize;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE core::uint Device::getMaxRenderMinBufferSize() const
    {
        return m_renderJobsWorkerMinBufferSize;
    }
}

