#pragma once

#include "editor/ImGui/Window/ImGuiWindow.h"

namespace vg::core
{
    class IOptions;
}

namespace vg::editor
{
    class ImGuiOptionsWindow : public ImGuiWindow
    {
    public:
        ImGuiOptionsWindow(const core::string & _icon, const core::string & _path, const core::string & _name, ImGuiWindow::Flags _flags);
        virtual core::IOptions * GetOptions() const = 0;
        virtual void DrawGUI() override;
    };
}