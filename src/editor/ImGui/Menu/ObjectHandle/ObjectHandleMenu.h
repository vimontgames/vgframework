#pragma once

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

    class ImGuiObjectHandleMenu
    {
    public:
        bool SelectUID(core::UID * _uid, IGameObject * _gameobject);
    };
}