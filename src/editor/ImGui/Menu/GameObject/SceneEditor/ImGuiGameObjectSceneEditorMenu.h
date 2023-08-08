#pragma once

#include "editor/ImGui/Menu/ImGuiMenu.h"

namespace vg::editor
{
    class ImGuiGameObjectSceneEditorMenu : public ImGuiMenu
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