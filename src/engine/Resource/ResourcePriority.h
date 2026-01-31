#pragma once

namespace vg::engine
{
    vg_enum_class(vg::engine, ResourcePriority, core::u16,
        World = 0,
        Scene,
        Prefab,
        Lens,
        Mesh,
        Material,
        Texture,
        Animation,
        Sound
    );
}