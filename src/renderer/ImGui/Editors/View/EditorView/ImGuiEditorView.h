#pragma once

#include "renderer/ImGui/Editors/View/ImGuiView.h"

namespace vg::renderer
{
    class ImGuiEditorView : public ImGuiView
    {
    public:
        ImGuiEditorView(const string & _name, Flags _flags);
        ~ImGuiEditorView(); 
    };
}