#pragma once

#include "editor/ImGui/Window/View/ImGuiView.h"

namespace vg::editor
{
    class ImGuiEditorView : public ImGuiView
    {
    public:
        VG_CLASS_DECL(ImGuiEditorView, ImGuiView);

        ImGuiEditorView(core::uint _index);
        ~ImGuiEditorView(); 

        renderer::ViewFlags GetViewFlags        () const final override;

        bool                ShowToolbar         () const final override;
        void                DrawToolbar         () final override;

        bool                ShowTitlebarMenu    () const final { return true; }
        void                DrawTitlebarMenu    () final override;

    };
}