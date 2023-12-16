#include "ImGuiRendererOptions.h"
#include "renderer/IRenderer.h"
#include "renderer/IRendererOptions.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiRendererOptions::ImGuiRendererOptions() :
        super(style::icon::Renderer, "", "Renderer", ImGuiWindow::StartVisible)
    {
    }

    //--------------------------------------------------------------------------------------
    core::IOptions * ImGuiRendererOptions::GetOptions() const
    {
        return Editor::get()->getRenderer()->GetOptions();
    }

    //--------------------------------------------------------------------------------------
    void ImGuiRendererOptions::DrawGUI()
    {
        super::DrawGUI();
    }
}