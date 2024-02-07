#pragma once

#include "editor/ImGui/Window/View/ImGuiView.h"

namespace vg::editor
{
    class ImGuiGameView : public ImGuiView
    {
    public:
        VG_CLASS_DECL(ImGuiGameView, ImGuiView);

        ImGuiGameView(core::uint _index);
        ~ImGuiGameView();
    };
}