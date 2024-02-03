#pragma once

#include "editor/ImGui/Window/View/ImGuiView.h"

namespace vg::editor
{
    class ImGuiEditorView : public ImGuiView
    {
    public:
        ImGuiEditorView(core::uint _index);
        ~ImGuiEditorView(); 
    };
}