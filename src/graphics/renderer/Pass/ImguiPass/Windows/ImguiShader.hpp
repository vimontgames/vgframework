#include "ImguiShader.h"

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    void ImguiShader::display()
    {
        if (ImGui::Begin("Shaders", &m_isVisible))
        {
            ImGui::Text("Press 'F6' to hot reload shaders");
        }
        ImGui::End();
    }
}