#include "ImguiInspector.h"

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    void ImguiInspector::display()
    {
        if (ImGui::Begin("Inspector", &m_isVisible))
        {
            if (ImguiEditor::getSelectedObject() != nullptr)
                ImguiEditor::displayObject(ImguiEditor::getSelectedObject());
        }
        ImGui::End();
    }
}