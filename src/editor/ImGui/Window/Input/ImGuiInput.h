#pragma once

#include "editor/ImGui/Window/ImGuiWindow.h"

namespace vg::editor
{
    class ImGuiInput : public ImGuiWindow
    {
    public:
        ImGuiInput();

        virtual void DrawGUI() final override;
    };
}