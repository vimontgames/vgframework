#pragma once

#include "editor/ImGui/Menu/ImGuiMenu.h"

namespace vg::core
{
    enum BaseSceneType : core::u8;
}

namespace vg::editor
{
    class ImGuiSceneMenu : public ImGuiMenu
    {
    public:
        enum MenuOption
        {
            None = 0,
            Save,
            Close
        };

        ImGuiSceneMenu(core::BaseSceneType _sceneType);
        Status Display(core::IObject * _object) final;

    private:
        core::BaseSceneType m_sceneType;
    };
}