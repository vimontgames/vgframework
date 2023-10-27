#pragma once

#include "editor/ImGui/Window/ImGuiWindow.h"

namespace vg::editor
{
    class ImGuiResource : public ImGuiWindow
    {
    public:
        ImGuiResource();

        virtual void DrawGUI() final;
    };
}