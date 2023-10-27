#pragma once

#include "editor/ImGui/Window/ImGuiWindow.h"

namespace vg::editor
{
    class ImGuiShader : public ImGuiWindow
    {
    public:
        ImGuiShader();

        virtual void DrawGUI() final override;
    };
}