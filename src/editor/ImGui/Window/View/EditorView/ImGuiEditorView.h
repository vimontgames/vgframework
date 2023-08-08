#pragma once

#include "editor/ImGui/Window/View/ImGuiView.h"

namespace vg::editor
{
    class ImGuiEditorView : public ImGuiView
    {
    public:
        ImGuiEditorView(const core::string & _name, Flags _flags);
        ~ImGuiEditorView(); 
    };
}