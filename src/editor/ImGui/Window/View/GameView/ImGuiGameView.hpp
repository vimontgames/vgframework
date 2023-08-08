#include "ImGuiGameView.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiGameView::ImGuiGameView(const string & _name, Flags _flags) :
        ImGuiView(_name, _flags, gfx::ViewTarget::Game)
    {

    }

    //--------------------------------------------------------------------------------------
    ImGuiGameView::~ImGuiGameView()
    {

    }
}