#pragma once

#include "graphics/renderer/ImGui/Menus/ImguiMenu.h"

namespace vg::graphics::renderer
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