#pragma once

#include "graphics/driver/FrameGraph/UserPass.h"

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    class ImguiPass : public driver::UserPass
    {
    public:
        void setup(double _dt) override;
        void draw(driver::CommandList * _cmdList) const override;

    private:
        void displayDebugWindow(double _dt);

    private:
        bool m_isDebugWindowVisible = true;
        bool m_isAboutWindowVisible = false;

        core::uint  m_captureFrameCounter = 0;

        double      m_accum = 0.0f;
        core::uint  m_frame = 0;
        float       m_dt = 0.0f;
        float       m_fps = 0.0f;
    };
}