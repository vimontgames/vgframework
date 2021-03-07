#pragma once

#include "core/Object/Object.h"

namespace vg::core
{
    class IModel : public Object
    {
    public:
        IModel(const string & _name) :
            Object(_name)
        {

        }
    };
}