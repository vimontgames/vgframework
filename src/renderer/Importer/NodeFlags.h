#pragma once

namespace vg::renderer
{
    vg_enum_class(NodeFlags, core::u32,
        UpperBody   = 0x00000001,
        LowerBody   = 0x00000002,

        Selected    = 0x80000000
    );

    struct NodeFlagsTable
    {
        const char * name;
        NodeFlags flags;
    };
}