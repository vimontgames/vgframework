#pragma once
#include "editor/ImGui/Window/ImGuiWindow.h"

namespace vg::editor
{
    class ImGuiGameOptions : public ImGuiOptionsWindow
    {
    public:
        using super = ImGuiOptionsWindow;
        ImGuiGameOptions();
        core::IOptions * GetOptions() const final override;
        void DrawGUI() final override;
    };
}