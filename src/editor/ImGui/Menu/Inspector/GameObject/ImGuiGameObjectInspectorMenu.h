#pragma once

#include "editor/ImGui/Menu/ImGuiMenu.h"

namespace vg::editor
{
    struct CategoryInfo
    {
        core::string name;
        bool show = true;
        core::vector<core::IClassDesc *> classes;
    };

    vg_enum_class(vg::editor, GameObjectInspectorMenuOption, core::u8,
        None = 0,
        AddComponent
    );

    class ImGuiGameObjectInspectorMenu : public ImGuiMenu
    {
    public:
        Status Display(core::IObject * _object);

        void addComponentPopup() { m_addComponent = true; }

    private:
        ImGuiTextFilter             m_addComponentFilter;
        const core::IClassDesc *    m_selectedClass = nullptr;
        core::vector<CategoryInfo>  m_categories;
        bool                        m_addComponent = false;
    };
}