#pragma once

#include "graphics/renderer/ImGui/Editors/ImguiEditor.h"
#include "graphics/renderer/ImGui/Menus/Scene/ImguiSceneMenu.h"
#include "graphics/renderer/ImGui/Menus/GameObject/SceneEditor/ImGuiGameObjectSceneEditorMenu.h"

namespace vg::graphics::renderer
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