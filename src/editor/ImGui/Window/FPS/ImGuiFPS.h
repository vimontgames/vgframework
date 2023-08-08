#pragma once

#include "editor/ImGui/Window/ImGuiWindow.h"

namespace vg::editor
{
    class ImGuiFPS : public ImGuiWindow
    {
    public:
        ImGuiFPS(const core::string & _name, Flags _flags) :
            ImGuiWindow(_name, _flags)
        {
        }

        virtual void    DrawGUI () final override;

    private:
        core::uint      m_captureFrameCounter = 0;
        double          m_accum = 0.0f;
        core::uint      m_frame = 0;
        float           m_dt = 0.0f;
        float           m_fps = 0.0f;
    };
}