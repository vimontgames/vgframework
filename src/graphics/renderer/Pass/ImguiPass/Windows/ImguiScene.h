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
        enum ContextMenu
        {
            None = 0,
            Save,
            Close
        };

        void            displayGameObject   (IGameObject * root);

    private:
        ContextMenu     m_selectedContextMenu = ContextMenu::None;
    };
}