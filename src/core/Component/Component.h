#pragma once

#include "core/IComponent.h"

namespace vg::core
{
    class GameObject;

    class Component : public IComponent
    {
    public:
                                Component           (const core::string & _name, IObject * _parent);
                                ~Component          ();

        static bool             registerProperties  (IClassDesc & _desc);

        void                    Update              (double _dt) { };
        const IGameObject *     GetGameObject       () const final;

        Flags                   GetFlags            () const final;
        void                    SetFlags            (Flags _flags, bool _enabled) final;

    public:
        VG_INLINE Flags         getFlags            () const;
        VG_INLINE void          setFlags            (Flags _flags, bool _enabled);
        VG_INLINE GameObject *  getGameObject       () const;

    private:
        Flags                   m_flags;
    };
}

#if VG_ENABLE_INLINE
#include "Component.inl"
#endif