#pragma once

#include "editor/ImGui/Window/ImGuiWindow.h"
#include "editor/ImGui/Menu/Scene/ImGuiSceneMenu.h"
#include "editor/ImGui/Menu/GameObject/SceneEditor/ImGuiGameObjectSceneEditorMenu.h"

namespace vg::editor
{
    class ImGuiScene : public ImGuiWindow
    {
    public:
        enum MenuOption
        {
            None = 0,
            AddScene,
            LoadScene,
        };

        ImGuiScene();

        virtual void                    DrawGUI             () final override;

    private:
        void                            displayGameObject   (core::IGameObject * root, core::uint * _count = nullptr);

    private:
        ImGuiSceneMenu                  m_sceneMenu;
        ImGuiGameObjectSceneEditorMenu  m_gameObjectMenu;
    };
}