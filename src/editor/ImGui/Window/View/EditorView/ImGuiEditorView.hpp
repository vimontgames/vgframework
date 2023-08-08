#include "ImGuiEditorView.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiEditorView::ImGuiEditorView(const string & _name, Flags _flags) :
        ImGuiView(_name, _flags, gfx::ViewTarget::Editor)
    {

    }

    //--------------------------------------------------------------------------------------
    ImGuiEditorView::~ImGuiEditorView()
    {

    }
}