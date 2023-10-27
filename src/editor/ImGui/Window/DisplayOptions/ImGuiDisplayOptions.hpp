#include "ImguiDisplayOptions.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiDisplayOptions::ImGuiDisplayOptions() :
        ImGuiWindow(style::icon::Display, "", "Display", ImGuiWindow::StartVisible)
    {
    }

    //--------------------------------------------------------------------------------------
    void ImGuiDisplayOptions::DrawGUI()
    {
        core::IObject * displayOptions = Editor::get()->getRenderer()->GetDisplayOptions();
        if (displayOptions)
        {
            if (ImGui::IconBegin(style::icon::Display, getName().c_str(), &m_isVisible))
                displayObject(displayOptions);

            ImGui::End();
        }
    }
}