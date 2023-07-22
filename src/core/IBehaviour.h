#pragma once
#include "core/Component/Component.h"

namespace vg::core
{
    class IBehaviour : public Component
    {
    public:
        IBehaviour(const core::string & _name, IObject * _parent) :
            Component(_name, _parent)
        {

        }
    };
}