#pragma once

#include "core/Object/Object.h"

namespace vg::core
{
    class IGameObject;

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

        virtual void                Update          (double _dt) = 0;
        virtual const IGameObject * GetGameObject   () const = 0;

        virtual Flags               GetFlags        () const = 0;
        virtual void                SetFlags        (Flags _flags, bool _enabled) = 0;
    };
}