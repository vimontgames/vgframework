#pragma once
#include "editor/ImGui/Window/ImGuiWindow.h"

namespace vg::editor
{
    class ImGuiAudioOptions : public ImGuiOptionsWindow
    {
    public:
        using super = ImGuiOptionsWindow;
        ImGuiAudioOptions();
        core::IOptions * GetOptions() const final override;
        void DrawGUI() final override;
    };
}