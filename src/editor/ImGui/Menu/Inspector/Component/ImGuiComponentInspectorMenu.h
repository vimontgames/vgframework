#pragma once

#include "editor/ImGui/Menu/ImGuiMenu.h"

namespace vg::editor
{
    class ImGuiComponentInspectorMenu : public ImGuiMenu
    {
    public:
        enum MenuOption
        {
            None = 0,
            RemoveComponent
        };

        Status Display(core::IObject * _object);
        void removeComponent(IComponent * _component)
        {
            m_componentToDelete = _component;
        }

        IComponent * m_componentToDelete = nullptr;
    };
}