#pragma once

#include "editor/ImGui/Window/ImGuiWindow.h"

namespace vg::editor
{
    class ImGuiEditorOptions : public ImGuiWindow
    {
    public:
        ImGuiEditorOptions();

        virtual void DrawGUI() final override;
    };
}