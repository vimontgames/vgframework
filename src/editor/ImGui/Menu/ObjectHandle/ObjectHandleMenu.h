#pragma once

#include "editor/ImGui/Menu/ImGuiMenu.h"

namespace vg::core
{
    using UID = core::u32;
}

namespace vg::editor
{
    vg_enum_class(ObjectHandleMenuOption, core::u8,
        None = 0,
        Select,
        Clear
    );

    class ImGuiObjectHandleMenu : public ImGuiMenu
    {
    public:
        bool SelectUID(core::UID * _uid, core::IGameObject * _gameobject, bool _open);
        ~ImGuiObjectHandleMenu();

    private:
        ImGuiTextFilter m_filter;
    };
}