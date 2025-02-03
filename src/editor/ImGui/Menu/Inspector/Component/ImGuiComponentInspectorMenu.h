#pragma once

#include "editor/ImGui/Menu/ImGuiMenu.h"

vg_enum_class_ns(vg::editor, InspectorMenuOption, core::u8,
    None = 0,
    RemoveComponent
);

namespace vg::editor
{
    class ImGuiComponentInspectorMenu : public ImGuiMenu
    {
    public:
        Status Display(core::IObject * _object);
        void removeComponent(IComponent * _component)
        {
            m_componentToDelete = _component;
        }

        IComponent * m_componentToDelete = nullptr;
        IObject * m_selectedObject = nullptr;
    };
}