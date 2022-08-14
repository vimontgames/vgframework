#include "ImguiDisplayOptions.h"

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    void ImguiDisplayOptions::display()
    {
        core::IObject * displayOptions = DisplayOptions::get();
        if (displayOptions)
        {
            if (ImGui::Begin("Display", &m_isVisible))
                displayObject(displayOptions);

            ImGui::End();
        }
    }
}