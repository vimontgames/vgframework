#pragma once

#include "core/Types/Types.h"

vg_enum_class_global(GameState, vg::core::u8,
    Invalid = 0,
    MainMenu,
    Playing,
    Paused
);

vg_enum_class_global(CharacterType , vg::core::u8,
    Neutral = 0,
    Player,
    Enemy
);

vg_enum_class_global(VehicleType, vg::core::u8,
    Default = 0
);

vg_enum_class_global(MoveState, vg::core::u8,
    Idle = 0,
    Walk,
    Run,
    Jump,
    Hurt,
    Die,
    Drive
);

vg_enum_class_global(FightState, vg::core::u8,
    None = 0,
    Hit,        // Upper body: Hitting with melee weapon (TODO: rename?)
    Shoot,      // Upper body: Shooting with pisol
    Kick        // Lower body: Kicking the ball
);

vg_enum_class_global(SoundState, vg::core::u8,
    None = 0,
    Hurt,
    Die,
    Hit
);

vg_enum_class_global(ItemType, vg::core::u8,
    Default = 0,
    Ball,
    Weapon,
    Projectile,
    Chest,
    Vehicle
);

vg_enum_class_global(BallType, vg::core::u8,
    Football,
    Rugby
);

vg_enum_class_global(WeaponType, vg::core::u8,
    Melee,  // A melee weapon (e.g. Sword)
    Pistol, // A weapon that shots projectiles (e.g. Gun)
    Fist    // A character is attacking with its bare hands
);

vg_enum_class_global(BreakableType, vg::core::u8,
    LootBox
);