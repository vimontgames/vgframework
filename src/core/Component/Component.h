#pragma once

#include "core/Object/Object.h"

namespace vg::core
{
    class Entity;

    class Component : public core::Object
    {
    public:
        Component(const core::string & _name);

        virtual void update(const Entity * _entity, double _dt) = 0;

    private:
        
    };
}