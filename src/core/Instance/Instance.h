#pragma once

#include "core/IInstance.h"

namespace vg::core
{
    class Instance : public IInstance
    {
    public:
        Instance(const string & _name, IObject * _parent);
    };
}