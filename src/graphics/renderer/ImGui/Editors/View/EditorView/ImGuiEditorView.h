#pragma once

#include "graphics/renderer/ImGui/Editors/View/ImGuiView.h"

namespace vg::graphics::renderer
{
    class ImGuiEditorView : public ImGuiView
    {
    public:
        ImGuiEditorView(const string & _name, Flags _flags);
        ~ImGuiEditorView(); 
    };
}