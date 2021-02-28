#pragma once

#include "core/Entity/Entity.h"

class PlayerEntity : public vg::core::Entity
{
public:
    PlayerEntity(const vg::core::string & _name);
    ~PlayerEntity();
};