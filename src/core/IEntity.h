#pragma once

#include "core/Object/Object.h"

namespace vg::core
{
    class IEntity : public Object
    {
    public:
        IEntity(const string & _name) :
            Object(_name)
        {

        }
    };
}