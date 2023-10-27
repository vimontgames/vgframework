#include "ImGuiEditorView.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiEditorView::ImGuiEditorView() :
        ImGuiView(style::icon::EditorView, "View", "Editor", ImGuiWindow::StartVisible | ImGuiWindow::AddMenuEntry, gfx::ViewTarget::Editor)
    {

    }

    //--------------------------------------------------------------------------------------
    ImGuiEditorView::~ImGuiEditorView()
    {

    }
}