#pragma once

#include "editor/ImGui/Window/ImGuiWindow.h"
#include "editor/ImGui/Menu/Scene/ImGuiSceneMenu.h"
#include "editor/ImGui/Menu/GameObject/SceneEditor/ImGuiGameObjectSceneEditorMenu.h"

namespace vg::editor
{
    class ImGuiGameObjectTree : public ImGuiWindow
    {
    public:
        enum MenuOption
        {
            None = 0,
            AddScene,
            LoadScene,
        };

        ImGuiGameObjectTree(const core::string& _icon, const core::string& _path, const core::string& _name, Flags _flags);

    protected:
        void                            displayGameObject(core::IGameObject* root, core::uint* _count = nullptr);

    protected:
        ImGuiSceneMenu                  m_sceneMenu;
        ImGuiGameObjectSceneEditorMenu  m_gameObjectMenu;
        core::IGameObject *             m_dragAndDropNodeTarget = nullptr;
        core::IGameObject *             m_dragAndDropInterlineTarget = nullptr;
    };
}