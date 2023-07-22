#include "ImGuiGameView.h"

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    ImGuiGameView::ImGuiGameView(const string & _name, Flags _flags) :
        ImGuiView(_name, _flags, driver::ViewType::Game)
    {

    }

    //--------------------------------------------------------------------------------------
    ImGuiGameView::~ImGuiGameView()
    {

    }
}