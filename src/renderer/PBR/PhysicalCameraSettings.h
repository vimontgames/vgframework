#pragma once

#include "gfx/IView.h"

namespace vg::renderer
{
    vg_enum_class(Sensor, core::u8,
        FullFrame = 0,
        APSC,
        MicroFourThirds
    );

    struct PhysicalCameraSettings
    {
        float               m_focalLength = 35.0f;
        Sensor              m_sensor = Sensor::FullFrame;
        bool                m_useCustomSensorSize = false;
        core::float2        m_customSensorSize = core::float2(36.0f, 24.0f);
        gfx::GateFitMode    m_gateFitMode = gfx::GateFitMode::Vertical;
        float               m_near = 0.1f;
        float               m_far = 1000.0f;

        inline core::float2 getSensorSize() const
        {
            if (m_useCustomSensorSize)
            {
                return m_customSensorSize;
            }
            else
            {
                switch (m_sensor)
                {
                    default:
                        VG_ASSERT_ENUM_NOT_IMPLEMENTED(m_sensor);

                    case Sensor::FullFrame:
                        return core::float2(36.0f, 24.0f);

                    case Sensor::APSC:
                        return core::float2(22.0f, 15.0f);

                    case Sensor::MicroFourThirds:
                        return core::float2(17.0f, 13.0f);
                }
            }
        }
    };
}