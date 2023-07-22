#include "ImGuiEditorView.h"

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    ImGuiEditorView::ImGuiEditorView(const string & _name, Flags _flags) :
        ImGuiView(_name, _flags, driver::ViewType::Editor)
    {

    }

    //--------------------------------------------------------------------------------------
    ImGuiEditorView::~ImGuiEditorView()
    {

    }
}