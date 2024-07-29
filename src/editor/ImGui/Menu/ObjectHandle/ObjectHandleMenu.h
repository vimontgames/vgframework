#pragma once

namespace vg::core
{
    using UID = core::u32;
}

namespace vg::editor
{
    class ImGuiObjectHandleMenu
    {
    public:
        enum MenuOption
        {
            None = 0,
            Select,
            Clear
        };

        bool SelectUID(core::UID * _uid, IGameObject * _gameobject);
    };
}