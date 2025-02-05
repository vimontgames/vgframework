#pragma once

namespace vg::core
{
    vg_enum_class(vg::core, EditingState, u8,
        Unknown = 0,
        BeginEdit,
        Editing,
        EndEdit
    );
}