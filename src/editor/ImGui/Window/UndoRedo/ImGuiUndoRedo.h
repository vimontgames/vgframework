#pragma once

#include "editor/ImGui/Window/ImGuiWindow.h"

namespace vg::editor
{
    class ImGuiUndoRedo final : public ImGuiWindow
    {
    public:
        ImGuiUndoRedo();
        void DrawGUI() final override;
    };
}