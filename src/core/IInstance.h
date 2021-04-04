#pragma once

#include "core/Object/Object.h"

namespace vg::core
{
    class IInstance : public Object
    {
    public:
        IInstance(const string & _name, IObject * _parent) :
            Object(_name, _parent)
        {

        }
    };
}