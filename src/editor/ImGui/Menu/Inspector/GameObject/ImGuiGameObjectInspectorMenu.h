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

    class ImGuiGameObjectInspectorMenu : public ImGuiMenu
    {
    public:
        enum MenuOption
        {
            None = 0,
            AddComponent
        };

        Status Display(core::IObject * _object);

        void addComponentPopup() { m_addComponent = true; }

    private:
        const core::IClassDesc *    m_selectedClass = nullptr;
        core::vector<CategoryInfo>  m_categories;
        bool                        m_addComponent = false;
    };
}