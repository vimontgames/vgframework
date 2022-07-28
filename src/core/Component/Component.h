#pragma once

#include "core/Object/Object.h"

#define VG_AUTO_REGISTER_COMPONENT(className) VG_AUTO_REGISTER_CLASS_EX(className, vg::core::IClassDesc::Flags::Component)

namespace vg::core
{
    class IEntity;

    class Component : public core::Object
    {
    public:
        Component(const core::string & _name, IObject * _parent);
        ~Component();

        virtual void update(double _dt) = 0;

        IEntity * getEntity() const;

    };
}