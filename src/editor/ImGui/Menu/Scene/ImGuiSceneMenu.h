#pragma once

#include "editor/ImGui/Menu/ImGuiMenu.h"

namespace vg::editor
{
    class ImGuiSceneMenu : public ImGuiMenu
    {
    public:
        enum MenuOption
        {
            None = 0,
            Save,
            Close
        };

        Status Display(core::IObject * _object) final;
    };
}