#pragma once

#include "editor/ImGui/Menu/ImGuiMenu.h"

namespace vg::core
{
    class IGameObject;
}

vg_enum_class_ns(vg::editor, GameObjectSceneEditorMenuOption, core::u8,
    None = 0,

    AddGameObject,
    AddChildGameObject,
    AddParentGameObject,

    EditPrefab,
    CreatePrefab,
    UnpackPrefab,
    AddPrefab,
    AddChildPrefab,
    ReplaceByPrefab,

    Delete,
    Rename
);

namespace vg::editor
{
    class ImGuiGameObjectSceneEditorMenu : public ImGuiMenu
    {
    public:
        Status Display(core::IObject * _object) override;

        core::IGameObject * m_RenamingGameObject = nullptr;
    };
}