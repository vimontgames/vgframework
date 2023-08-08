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

        ImGuiScene(const core::string & _name, Flags _flags) :
            ImGuiWindow(_name, _flags)
        {
        }

        virtual void                    DrawGUI             () final override;

    private:
        void                            displayGameObject   (core::IGameObject * root);

    private:
        ImGuiSceneMenu                  m_sceneMenu;
        ImGuiGameObjectSceneEditorMenu  m_gameObjectMenu;
    };
}