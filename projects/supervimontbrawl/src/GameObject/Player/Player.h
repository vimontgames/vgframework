#pragma once

#include "core/GameObject/GameObject.h"

class Player : public vg::core::GameObject
{
public:
    const char * getClassName() const final { return "Player"; }

    Player(const vg::core::string & _name, vg::core::IObject * _parent);
    ~Player();
};