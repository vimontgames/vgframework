#pragma once

#include "renderer/ImGui/Editors/View/ImGuiView.h"

namespace vg::renderer
{
    class ImGuiGameView : public ImGuiView
    {
    public:
        ImGuiGameView(const string & _name, Flags _flags);
        ~ImGuiGameView();
    };
}