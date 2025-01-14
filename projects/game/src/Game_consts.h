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
    Hit,        // Upper body: Hitting with melee weapon (TODO: rename?)
    Shoot,      // Upper body: Shooting with pisol
    Kick        // Lower body: Kicking the ball
);

vg_enum_class(SoundState, vg::core::u8,
    None = 0,
    Hurt,
    Die
);

vg_enum_class(ItemType, vg::core::u8,
    Default = 0,
    Ball,
    Weapon,
    Projectile,
    Chest
);

vg_enum_class(BallType, vg::core::u8,
    Football,
    Rugby
);

vg_enum_class(WeaponType, vg::core::u8,
    Melee,
    Pistol
);

vg_enum_class(ChestType, vg::core::u8,
    Default
);