#include "ImGuiGameView.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    ImGuiGameView::ImGuiGameView(const string & _name, Flags _flags) :
        ImGuiView(_name, _flags, gfx::ViewType::Game)
    {

    }

    //--------------------------------------------------------------------------------------
    ImGuiGameView::~ImGuiGameView()
    {

    }
}