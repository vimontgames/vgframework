#pragma once

#include "core/IComponent.h"

#define VG_AUTO_REGISTER_COMPONENT(className) VG_AUTO_REGISTER_CLASS_EX(className, vg::core::IClassDesc::Flags::Component)

namespace vg::core
{
    class IEntity;

    class Component : public IComponent
    {
    public:
                            Component           (const core::string & _name, IObject * _parent);
                            ~Component          ();

        static bool         registerProperties  (IClassDesc & _desc);

        void                update              (double _dt) { };
        const IEntity *     getEntity           () const final;

        Flags               getFlags            () const final;
        void                setFlags            (Flags flags, bool enabled) final;

    private:
        Flags               m_flags;
    };
}