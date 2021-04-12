#pragma once

#include "core/Instance/Instance.h"

namespace vg::core
{
    class ISector;

    class IEntity : public Instance
    {
    public:
        IEntity(const core::string & _name, IObject * _parent) :
            Instance(_name, _parent)
        {

        }

        virtual ISector *   getSector() const = 0;
    };
}