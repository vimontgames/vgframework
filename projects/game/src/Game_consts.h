#pragma once

#include "core/Types/Types.h"

enum CharacterType : vg::core::u8
{
    Player = 0,
    Enemy
};

enum CharacterState : vg::core::u8
{
    Idle = 0,
    Walking,
    Running,
    Jumping
};