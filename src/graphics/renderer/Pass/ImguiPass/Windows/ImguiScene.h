#pragma once

#include "ImguiEditor.h"
#include "graphics/renderer/Pass/ImguiPass/Menus/ImguiSceneMenu.h"
#include "graphics/renderer/Pass/ImguiPass/Menus/ImguiGameObjectMenu.h"

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

        ImguiScene(const string & _name, Flags _flags) :
            ImguiEditor(_name, _flags)
        {
        }

        virtual void    display             () final;

    private:
        void                    displayGameObject   (IGameObject * root);

    private:
        ImguiSceneMenu          m_sceneMenu;
        ImguiGameObjectMenu     m_gameObjectMenu;
    };
}