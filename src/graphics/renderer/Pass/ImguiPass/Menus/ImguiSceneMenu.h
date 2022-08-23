#pragma once

#include "ImguiMenu.h"

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

        void Display(core::IObject * _object) final;
    };
}