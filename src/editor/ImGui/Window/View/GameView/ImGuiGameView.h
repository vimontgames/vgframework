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

        bool    ShowToolbar         () const final override;
        void    DrawToolbar         () final override;

        bool    ShowTitlebarMenu    () const final override { return true; };
        void    DrawTitlebarMenu    () final override;
    };
}