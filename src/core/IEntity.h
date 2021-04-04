#pragma once

#include "core/Object/Object.h"

namespace vg::core
{
    class ISector;

    class IEntity : public Object
    {
    public:
        IEntity(const core::string & _name, IObject * _parent) :
            Object(_name, _parent)
        {

        }

        virtual ISector *   getSector() const = 0;
    };
}