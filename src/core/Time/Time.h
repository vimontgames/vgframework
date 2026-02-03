#pragma once

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    // Store various times used by various modules
    // This is Engine's duty to update this structure each frame and pass to other modules
    //--------------------------------------------------------------------------------------
    struct Time
    {
        float m_realDeltaTime = 0.0f;
        float m_scaledDeltaTime = 0.0f;
        float m_enlapsedTimeSinceStartReal = 0.0f;
        float m_enlapsedTimeSinceStartScaled = 0.0f;
        float m_fps = 0.0f;
        float m_gpu = 0.0f;
        float m_gpuWait = 0.0f;

        struct
        {
            float m_dt = 0.0f;
            float m_fps = 0.0f;
            float m_gpu = 0.0f;
            float m_gpuWait = 0.0f;
        } smoothed;
    };
}