#pragma once

#include "core/Object/Object.h"

namespace vg::core
{
    class IEntity;

    class IComponent : public core::Object
    {
    public:
        enum Flags : u32
        {
            Enabled = 0x00000001
        };

        IComponent(const core::string & _name, IObject * _parent) :
            core::Object(_name, _parent)
        {

        }

        virtual void            update      (double _dt) = 0;
        virtual const IEntity * getEntity   () const = 0;

        virtual Flags           getFlags    () const = 0;
        virtual void            setFlags    (Flags flags, bool enabled) = 0;
    };
}