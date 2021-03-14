#pragma once

#include "core/Entity/Entity.h"

class PlayerEntity : public vg::core::Entity
{
public:
    const char * getClassName() const final { return "PlayerEntity"; }

    PlayerEntity(const vg::core::string & _name);
    ~PlayerEntity();
};