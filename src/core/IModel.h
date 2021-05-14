#pragma once

#include "core/Object/Object.h"

namespace vg::core
{
    class IModel : public ObjectPointer
    {
    public:
        IModel(const string & _name, IObject * _parent) :
            ObjectPointer(_name, _parent)
        {

        }
    };
}