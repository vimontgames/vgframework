#pragma once

vg_enum_class_ns(vg::renderer, BodyPartFlags, core::u32,
    UpperBody = 0x00000001,
    LowerBody = 0x00000002
);

namespace vg::renderer
{
    struct NodeFlagsTable
    {
        const char * name;
        BodyPartFlags flags;
    };
}