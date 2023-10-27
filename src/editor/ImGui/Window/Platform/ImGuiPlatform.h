#pragma once

#include "editor/ImGui/Window/ImGuiWindow.h"

namespace vg::editor
{
    class ImGuiPlatform : public ImGuiWindow
    {
    public:
        ImGuiPlatform();

        virtual void DrawGUI() final;
    };
}