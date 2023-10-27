#pragma once

#include "editor/ImGui/Window/ImGuiWindow.h"

namespace vg::editor
{
    class ImGuiDisplayOptions: public ImGuiWindow
    {
    public:
        ImGuiDisplayOptions();

        virtual void DrawGUI() final override;
    };
}