#include "ImguiDisplayOptions.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    void ImGuiDisplayOptions::DrawGUI()
    {
        core::IObject * displayOptions = Editor::get()->getRenderer()->GetDisplayOptions();
        if (displayOptions)
        {
            if (ImGui::IconBegin(style::icon::Display, "Display", &m_isVisible))
                displayObject(displayOptions);

            ImGui::End();
        }
    }
}