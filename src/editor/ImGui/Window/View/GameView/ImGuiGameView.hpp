#include "ImGuiGameView.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiGameView::ImGuiGameView(uint _index) :
        ImGuiView(style::icon::GameView, "View", "Game", ImGuiWindow::StartVisible | ImGuiWindow::AddMenuEntry, gfx::ViewTarget::Game, _index)
    {

    }

    //--------------------------------------------------------------------------------------
    ImGuiGameView::~ImGuiGameView()
    {

    }
}