#pragma once

#include "editor/ImGui/Window/ImGuiWindow.h"

namespace vg::editor
{
    class ImGuiStatistics : public ImGuiWindow
    {
    public:
        ImGuiStatistics();
        void DrawGUI() final override;
    };
}