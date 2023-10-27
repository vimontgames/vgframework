#pragma once

#include "editor/ImGui/Window/ImGuiWindow.h"

namespace vg::editor
{
    class ImGuiAbout : public ImGuiWindow
    {
    public:
        ImGuiAbout();

        virtual void DrawGUI() final override;
    };
}