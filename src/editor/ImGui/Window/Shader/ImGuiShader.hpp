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
            renderer::IRenderer * renderer = Editor::get()->getRenderer();

            if (ImGui::TooltipButton(fmt::sprintf("%s Reload Shaders", style::icon::Reload).c_str(), renderer != nullptr, renderer != nullptr, "Reload Shaders (F6)", style::button::SizeLarge))
                renderer->updateShaders();

            ImGui::Separator();   

            ImGui::BeginChild(ImGui::getObjectLabel("ChildWindow", this).c_str());
            {
            }
            ImGui::EndChild();
        }
        ImGui::End();
    }
}