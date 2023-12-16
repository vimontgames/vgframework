#pragma once
#include "editor/ImGui/Window/ImGuiWindow.h"

namespace vg::editor
{
    class ImGuiPhysicsOptions : public ImGuiOptionsWindow
    {
    public:
        using super = ImGuiOptionsWindow;
        ImGuiPhysicsOptions();
        core::IOptions * GetOptions() const final override;
        void DrawGUI() final override;
    };
}