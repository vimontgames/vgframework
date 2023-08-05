#pragma once

#include "renderer/ImGui/Menus/ImguiMenu.h"

namespace vg::renderer
{
    class ImguiSceneMenu : public ImguiMenu
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