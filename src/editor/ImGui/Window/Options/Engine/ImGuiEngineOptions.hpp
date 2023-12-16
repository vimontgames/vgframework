#include "ImGuiEngineOptions.h"
#include "engine/IEngineOptions.h"
#include "engine/IEngine.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiEngineOptions::ImGuiEngineOptions() :
        super(style::icon::Engine, "", "Engine", ImGuiWindow::StartVisible)
    {
    }

    //--------------------------------------------------------------------------------------
    core::IOptions * ImGuiEngineOptions::GetOptions() const
    {
        return Editor::get()->getEngine()->GetOptions();
    }

    //--------------------------------------------------------------------------------------
    void ImGuiEngineOptions::DrawGUI()
    {
        super::DrawGUI();
    }
}