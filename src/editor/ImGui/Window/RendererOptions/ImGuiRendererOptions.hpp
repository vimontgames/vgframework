#include "ImGuiRendererOptions.h"
#include "renderer/IRendererOptions.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiRendererOptions::ImGuiRendererOptions() :
        ImGuiWindow(style::icon::Renderer, "", "Renderer", ImGuiWindow::StartVisible)
    {
    }

    //--------------------------------------------------------------------------------------
    void ImGuiRendererOptions::DrawGUI()
    {
        auto * options = Editor::get()->getRenderer()->GetOptions();
        if (options)
        {
            if (ImGui::IconBegin(style::icon::Renderer, getName().c_str(), &m_isVisible))
                displayObject(options);

            ImGui::End();
        }
    }
}