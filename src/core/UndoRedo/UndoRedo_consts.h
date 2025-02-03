#pragma once

vg_enum_class_ns(vg::core, EditingState, core::u8,
    Unknown = 0,
    BeginEdit,
    Editing,
    EndEdit
);