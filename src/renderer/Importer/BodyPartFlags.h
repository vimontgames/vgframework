#pragma once

namespace vg::renderer
{
    vg_enum_class(vg::renderer, BodyPartFlags, core::u32,
        UpperBody   = 0x00000001,
        LowerBody   = 0x00000002
    );

    struct NodeFlagsTable
    {
        const char * name;
        BodyPartFlags flags;
    };
}