#pragma once

#include "ImguiMenu.h"

namespace vg::graphics::renderer
{
    class ImguiGameObjectMenu : public ImguiMenu
    {
    public:
        enum MenuOption
        {
            None = 0,
            AddChild,
            Delete
        };

        Status Display(core::IObject * _object) final;
    };
}