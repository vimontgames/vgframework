#include "ImGuiEditorView.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiEditorView::ImGuiEditorView(uint _index) :
        ImGuiView(style::icon::EditorView, "View", "Editor", ImGuiWindow::StartVisible | ImGuiWindow::AddMenuEntry, gfx::ViewTarget::Editor, _index)
    {

    }

    //--------------------------------------------------------------------------------------
    ImGuiEditorView::~ImGuiEditorView()
    {

    }
}