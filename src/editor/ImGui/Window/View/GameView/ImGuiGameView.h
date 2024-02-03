#pragma once

#include "editor/ImGui/Window/View/ImGuiView.h"

namespace vg::editor
{
    class ImGuiGameView : public ImGuiView
    {
    public:
        ImGuiGameView(core::uint _index);
        ~ImGuiGameView();
    };
}