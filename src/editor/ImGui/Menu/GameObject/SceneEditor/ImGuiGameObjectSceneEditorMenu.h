#pragma once

#include "editor/ImGui/Menu/ImGuiMenu.h"

namespace vg::core
{
    class IGameObject;
}

namespace vg::editor
{
    class ImGuiGameObjectSceneEditorMenu : public ImGuiMenu
    {
    public:
        enum MenuOption
        {
            None = 0,
            AddChild,
            Delete,
            Rename
        };

        Status Display(core::IObject * _object) override;

        core::IGameObject * m_RenamingGameObject = nullptr;
    };
}