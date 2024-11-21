#pragma once

#include "gfx/IView.h"

namespace vg::renderer
{
    struct PhysicalCameraSettings
    {
        float               m_focalLength = 35.0f;
        gfx::GateFitMode    m_gateFitMode = gfx::GateFitMode::Vertical;
        float               m_near = 0.1f;
        float               m_far = 1000.0f;
    };
}