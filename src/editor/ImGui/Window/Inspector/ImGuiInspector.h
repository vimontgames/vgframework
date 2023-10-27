#pragma once

#include "editor/ImGui/Window/ImGuiWindow.h"

namespace vg::editor
{
    class ImGuiInspector : public ImGuiWindow
    {
    public:
        ImGuiInspector();

        virtual void DrawGUI() final override;
    };
}