#pragma once

#include "editor/ImGui/Window/ImGuiWindow.h"

namespace vg::editor
{
    class ImGuiFPS : public ImGuiWindow
    {
    public:
        ImGuiFPS();

        virtual void    DrawGUI () final override;

    private:
        core::uint      m_captureFrameCounter = 0;
    };
}