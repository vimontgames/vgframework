#include "ImGuiEditorOptions.h"
#include "editor/IEditorOptions.h"
#include "editor/Editor.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiEditorOptions::ImGuiEditorOptions() :
        super(style::icon::Editor, "", "Editor", ImGuiWindow::StartVisible)
    {
    }

    //--------------------------------------------------------------------------------------
    core::IOptions * ImGuiEditorOptions::GetOptions() const
    {
        return Editor::get()->GetOptions();
    }

    //--------------------------------------------------------------------------------------
    void ImGuiEditorOptions::DrawGUI()
    {
        super::DrawGUI();
    }
}