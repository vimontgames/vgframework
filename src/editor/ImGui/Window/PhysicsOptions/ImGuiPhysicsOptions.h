#pragma once

#include "editor/ImGui/Window/ImGuiWindow.h"

namespace vg::editor
{
    class ImGuiPhysicsOptions : public ImGuiWindow
    {
    public:
        ImGuiPhysicsOptions();

        virtual void DrawGUI() final override;
    };
}