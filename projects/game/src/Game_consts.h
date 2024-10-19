#pragma once

#include "core/Types/Types.h"

vg_enum_class(CharacterType , vg::core::u8,
    Neutral = 0,
    Player,
    Enemy
);

vg_enum_class(MoveState, vg::core::u8,
    Idle = 0,
    Walk,
    Run,
    Jump,
    Hurt,
    Die
);

vg_enum_class(FightState, vg::core::u8,
    None = 0,
    Hit,
    Kick
);

vg_enum_class(SoundState, vg::core::u8,
    None = 0,
    Hit,
    Die
);

vg_enum_class(ItemType, vg::core::u8,
    Default = 0,
    Ball,
    Weapon
);