#pragma once

#include "editor/ImGui/Menu/ImGuiMenu.h"

namespace vg::editor
{
    class ImGuiGameObjectInspectorMenu : public ImGuiMenu
    {
    public:
        enum MenuOption
        {
            None = 0,
            AddComponent
        };

        Status Display(core::IObject * _object);

    private:
        const char * m_selectedClassName = nullptr;
    };
}