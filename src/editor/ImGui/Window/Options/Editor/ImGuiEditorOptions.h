#pragma once
#include "editor/ImGui/Window/Options/ImGuiOptionsWindow.h"

namespace vg::editor
{
    class ImGuiEditorOptions : public ImGuiOptionsWindow
    {
    public:
        using super = ImGuiOptionsWindow;
        ImGuiEditorOptions();
        core::IOptions * GetOptions() const final override;
        void DrawGUI() final override;
    };
}