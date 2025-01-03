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
        return m_maxRenderJobCount;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE bool Device::getRenderJobsMainThreadOnly() const
    {
        return m_renderJobsMainThreadOnly;
    }
}

