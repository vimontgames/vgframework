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
}

