#pragma once

#include "renderer/ImGui/Editors/ImguiEditor.h"
#include "renderer/ImGui/Menus/Scene/ImguiSceneMenu.h"
#include "renderer/ImGui/Menus/GameObject/SceneEditor/ImGuiGameObjectSceneEditorMenu.h"

namespace vg::renderer
{
    class ImguiScene : public ImguiEditor
    {
    public:
        enum MenuOption
        {
            None = 0,
            AddScene,
            LoadScene,
        };

        ImguiScene(const core::string & _name, Flags _flags) :
            ImguiEditor(_name, _flags)
        {
        }

        virtual void                    display             () final;

    private:
        void                            displayGameObject   (core::IGameObject * root);

    private:
        ImguiSceneMenu                  m_sceneMenu;
        ImguiGameObjectSceneEditorMenu  m_gameObjectMenu;
    };
}