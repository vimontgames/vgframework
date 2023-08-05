#include "ImGuiEditorView.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    ImGuiEditorView::ImGuiEditorView(const string & _name, Flags _flags) :
        ImGuiView(_name, _flags, gfx::ViewType::Editor)
    {

    }

    //--------------------------------------------------------------------------------------
    ImGuiEditorView::~ImGuiEditorView()
    {

    }
}