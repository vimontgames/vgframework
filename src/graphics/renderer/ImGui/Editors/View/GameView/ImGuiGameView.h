#pragma once

#include "graphics/renderer/ImGui/Editors/View/ImGuiView.h"

namespace vg::graphics::renderer
{
    class ImGuiGameView : public ImGuiView
    {
    public:
        ImGuiGameView(const string & _name, Flags _flags);
        ~ImGuiGameView();
    };
}