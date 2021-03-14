#pragma once

#include "graphics/driver/FrameGraph/UserPass.h"

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    class ImguiPass : public driver::UserPass
    {
    public:
        const char * getClassName() const final { return "ImguiPass"; }

        void setup(double _dt) override;
        void draw(driver::CommandList * _cmdList) const override;

    private:
        void displayOptionsWindow();
        void displayDebugWindow(double _dt);

        void displayObject(core::IObject * _object, bool * _open);

    private:
        bool m_isDisplayOptionsWindowsVisible = false;
        bool m_isDebugWindowVisible = true;
        bool m_isAboutWindowVisible = false;

        core::uint  m_captureFrameCounter = 0;

        double      m_accum = 0.0f;
        core::uint  m_frame = 0;
        float       m_dt = 0.0f;
        float       m_fps = 0.0f;
    };
}