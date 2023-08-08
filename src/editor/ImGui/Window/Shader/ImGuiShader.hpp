#include "ImGuiShader.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    void ImGuiShader::DrawGUI()
    {
        if (ImGui::IconBegin(style::icon::Shaders, "Shaders", &m_isVisible))
        {
            ImGui::Text("Press 'F6' to hot reload shaders");
        }
        ImGui::End();
    }
}