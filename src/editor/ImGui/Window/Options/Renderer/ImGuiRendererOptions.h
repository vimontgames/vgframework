#pragma once
#include "editor/ImGui/Window/ImGuiWindow.h"

namespace vg::editor
{
    class ImGuiRendererOptions : public ImGuiOptionsWindow
    {
    public:
        using super = ImGuiOptionsWindow;
        ImGuiRendererOptions();
        core::IOptions * GetOptions() const final override;
        void DrawGUI() final override;
    };
}