#pragma once

#include "graphics/driver/FrameGraph/UserPass.h"

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    class ImguiPass : public driver::UserPass
    {
    public:
        void setup() override;
        void draw(driver::CommandList * _cmdList) const override;

    private:
        void displayDebugWindow();

    private:
        bool m_isDebugWindowVisible = true;
        bool m_isAboutWindowVisible = false;
        core::uint m_captureFrameCounter = 0;
    };
}