#pragma once

#include "core/Types/Types.h"

vg_enum(CharacterType , vg::core::u8,
    Neutral = 0,
    Player,
    Enemy
);

vg_enum(CharacterState, vg::core::u8,
    Idle = 0,
    Walking,
    Running,
    Jumping
);

vg_enum(ItemType, vg::core::u8,
    Ball = 0
);