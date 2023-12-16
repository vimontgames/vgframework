#pragma once
#include "editor/ImGui/Window/ImGuiWindow.h"

namespace vg::editor
{
    class ImGuiEngineOptions : public ImGuiOptionsWindow
    {
    public:
        using super = ImGuiOptionsWindow;
        ImGuiEngineOptions();
        core::IOptions * GetOptions() const final override;
        void DrawGUI() final override;
    };
}