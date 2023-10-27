#include "ImGuiShader.h"
#include "gfx/IShaderManager.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiShader::ImGuiShader() :
        ImGuiWindow(style::icon::Shaders, "", "Shaders", ImGuiWindow::StartVisible | ImGuiWindow::AddMenuEntry)
    {
    }

    //--------------------------------------------------------------------------------------
    void ImGuiShader::DrawGUI()
    {
        IShaderManager * sm = Editor::get()->getRenderer()->GetShaderManager();

        if (ImGui::IconBegin(style::icon::Shaders, "Shaders", &m_isVisible))
        {
            ImGui::Text("Press 'F6' to ");
            ImGui::SameLine();
            renderer::IRenderer * renderer = Editor::get()->getRenderer();
            if (ImGui::ButtonEx("Update Shaders", renderer != nullptr, "Compile all modified shaders"))
                renderer->updateShaders();
        }
        ImGui::End();
    }
}