#pragma once

#include "editor/ImGui/Window/ImGuiWindow.h"

namespace vg::editor
{
    class ImGuiPlugin : public ImGuiWindow
    {
    public:
        ImGuiPlugin(const core::string & _name, Flags _flags = ImGuiWindow::Flags::None) :
            ImGuiWindow(_name, _flags)
        {
        }

        virtual void DrawGUI() final override;
    };
}