#include "ImGuiGameOptions.h"
#include "application/IGame.h"
#include "application/IGameOptions.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiGameOptions::ImGuiGameOptions() :
        super(style::icon::Game, "", "Game", ImGuiWindow::StartVisible)
    {
    }

    //--------------------------------------------------------------------------------------
    core::IOptions * ImGuiGameOptions::GetOptions() const
    {
        return Editor::get()->getEngine()->GetGame()->GetOptions();
    }

    //--------------------------------------------------------------------------------------
    void ImGuiGameOptions::DrawGUI()
    {
        super::DrawGUI();
    }
}