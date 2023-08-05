#include "ImguiDisplayOptions.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    void ImguiDisplayOptions::display()
    {
        core::IObject * displayOptions = DisplayOptions::get();
        if (displayOptions)
        {
            if (ImGui::IconBegin(Editor::Icon::Display, "Display", &m_isVisible))
                displayObject(displayOptions);

            ImGui::End();
        }
    }
}