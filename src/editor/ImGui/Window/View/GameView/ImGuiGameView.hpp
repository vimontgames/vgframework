#include "ImGuiGameView.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiGameView::ImGuiGameView() :
        ImGuiView(style::icon::GameView, "View", "Game", ImGuiWindow::StartVisible | ImGuiWindow::AddMenuEntry, gfx::ViewTarget::Game)
    {

    }

    //--------------------------------------------------------------------------------------
    ImGuiGameView::~ImGuiGameView()
    {

    }
}