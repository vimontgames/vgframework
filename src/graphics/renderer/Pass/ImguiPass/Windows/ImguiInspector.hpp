#include "ImguiInspector.h"

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    void ImguiInspector::display()
    {
        if (ImGui::IconBegin(Editor::Icon::Inspector, "Inspector", &m_isVisible))
        {
            auto object = ImguiEditor::getSelectedObject();
            if (nullptr != object)
            {
                ImguiEditor::displayObject(object);
            }
        }
        ImGui::End();
    }
}