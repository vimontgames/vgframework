#include "ImguiInspector.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    void ImGuiInspector::DrawGUI()
    {
        ImGui::PushID("ImGuiInspector");
        if (ImGui::IconBegin(style::icon::Inspector, "Inspector", &m_isVisible))
        {
            auto * object = ImGuiWindow::getSelectedObject();
            if (nullptr != object)
            {
                ImGuiWindow::displayObject(object);
            }
        }
        ImGui::End();
        ImGui::PopID();
    }
}