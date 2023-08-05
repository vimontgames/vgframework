#pragma once

#include "renderer/ImGui/Menus/ImguiMenu.h"

namespace vg::renderer
{
    class ImguiGameObjectSceneEditorMenu : public ImguiMenu
    {
    public:
        enum MenuOption
        {
            None = 0,
            AddChild,
            Delete
        };

        Status Display(core::IObject * _object) override;
    };
}