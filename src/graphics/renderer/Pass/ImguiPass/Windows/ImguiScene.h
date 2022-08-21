#pragma once

#include "ImguiEditor.h"

namespace vg::graphics::renderer
{
    class ImguiScene : public ImguiEditor
    {
    public:
        ImguiScene(const string & _name, Flags _flags) :
            ImguiEditor(_name, _flags)
        {
        }

        virtual void    display             () final;

    private:
        enum class SceneMenu
        {
            None = 0,
            Save,
            Close
        };

        enum class GameObjectMenu
        {
            None = 0,
            AddGameObject
        };

        void            displayGameObject   (IGameObject * root);

    private:
        SceneMenu       m_sceneMenu = SceneMenu::None;
        GameObjectMenu  m_gameObjectMenu = GameObjectMenu::None;
    };
}