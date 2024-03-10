#pragma once

#include "editor/ImGui/Window/ImGuiWindow.h"

namespace vg::editor
{
    class ImGuiTime : public ImGuiWindow
    {
    public:
        ImGuiTime();

        virtual void DrawGUI() final override;
    };
}