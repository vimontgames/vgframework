#pragma once

#include "editor/ImGui/Menu/ImGuiMenu.h"

namespace vg::core
{
    enum BaseSceneType : core::u8;
}

vg_enum_ns(vg::editor, SceneMenuOption, core::u8,
    None = 0,
    Save,
    Close
);

namespace vg::editor
{
    class ImGuiSceneMenu : public ImGuiMenu
    {
    public:
        ImGuiSceneMenu(core::BaseSceneType _sceneType);
        Status Display(core::IObject * _object) final;

    private:
        core::BaseSceneType m_sceneType;
    };
}