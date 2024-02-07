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
    };
}