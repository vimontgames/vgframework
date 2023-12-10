#include "ImGuiEditorOptions.h"
#include "editor/IEditorOptions.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiEditorOptions::ImGuiEditorOptions() :
        ImGuiWindow(style::icon::Editor, "", "Editor", ImGuiWindow::StartVisible)
    {
    }

    //--------------------------------------------------------------------------------------
    void ImGuiEditorOptions::DrawGUI()
    {
        auto * options = Editor::get()->GetOptions();
        if (options)
        {
            if (ImGui::IconBegin(style::icon::Editor, getName().c_str(), &m_isVisible))
            {
                ObjectContext context;
                displayObject(options, context);
            }

            ImGui::End();
        }
    }
}