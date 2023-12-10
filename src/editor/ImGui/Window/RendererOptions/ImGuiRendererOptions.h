#pragma once

#include "editor/ImGui/Window/ImGuiWindow.h"

namespace vg::editor
{
    class ImGuiRendererOptions: public ImGuiWindow
    {
    public:
        ImGuiRendererOptions();

        virtual void DrawGUI() final override;
    };
}