#pragma once

#include "editor/ImGui/Window/View/ImGuiView.h"

namespace vg::editor
{
    class ImGuiGameView : public ImGuiView
    {
    public:
        ImGuiGameView(const core::string & _name, Flags _flags);
        ~ImGuiGameView();
    };
}