#pragma once

#include "editor/ImGui/Menu/ImGuiMenu.h"

namespace vg::core
{
    enum SceneType : core::u8;
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

        ImGuiSceneMenu(core::SceneType _sceneType);
        Status Display(core::IObject * _object) final;

    private:
        core::SceneType m_sceneType;
    };
}