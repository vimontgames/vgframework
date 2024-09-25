#pragma once

namespace vg::core
{
    vg_enum_class(EditingState, core::u8,
        Unknown = 0,
        BeginEdit,
        Editing,
        EndEdit
    );
}