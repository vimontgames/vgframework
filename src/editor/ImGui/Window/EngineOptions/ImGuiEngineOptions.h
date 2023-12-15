#pragma once

#include "editor/ImGui/Window/ImGuiWindow.h"

namespace vg::editor
{
    class ImGuiEngineOptions : public ImGuiWindow
    {
    public:
        ImGuiEngineOptions();

        virtual void DrawGUI() final override;
    };
}