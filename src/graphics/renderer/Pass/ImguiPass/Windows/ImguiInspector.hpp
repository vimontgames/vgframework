#include "ImguiInspector.h"

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    void ImguiInspector::display()
    {
        if (ImGui::Begin("Inspector", &m_isVisible))
        {
            auto object = ImguiEditor::getSelectedObject();
            if (nullptr != object)
            {
                if (ImGui::CollapsingHeader(object->getClassName(), nullptr, ImGuiTreeNodeFlags_DefaultOpen))
                {
                    ImguiEditor::displayObject(object);
                }
            }
        }
        ImGui::End();
    }
}