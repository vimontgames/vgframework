#include "ImguiShader.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    void ImguiShader::display()
    {
        if (ImGui::IconBegin(Editor::Icon::Shaders, "Shaders", &m_isVisible))
        {
            ImGui::Text("Press 'F6' to hot reload shaders");
        }
        ImGui::End();
    }
}