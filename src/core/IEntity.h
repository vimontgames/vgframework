#pragma once

#include "core/Instance/Instance.h"

namespace vg::core
{
    class ISector;

    class IEntity : public Instance
    {
    public:
        enum Flags : u32
        {
            Enabled  = 0x00000001,
            Selected = 0x00000002
        };

        IEntity(const core::string & _name, IObject * _parent) :
            Instance(_name, _parent)
        {

        }

        virtual ISector *       getSector() const = 0;
        virtual Flags           getFlags() const = 0;
        virtual void            setFlags(Flags flags, bool enabled) = 0;
    };
}