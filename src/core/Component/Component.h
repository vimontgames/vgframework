#pragma once

#include "core/IComponent.h"

namespace vg::core
{
    class GameObject;

    class Component : public IComponent
    {
    public:
        VG_CLASS_DECL(Component, IComponent);

                                Component               (const core::string & _name, IObject * _parent);
                                ~Component              ();

        void                    FixedUpdate             (float _dt) override;
        void                    Update                  (float _dt) override;
        void                    LateUpdate              (float _dt) override;

        const IGameObject *     GetGameObject           () const final override;
        IGameObject *           GetGameObject           () final override;

        Flags                   GetFlags                () const final override;
        void                    SetFlags                (Flags _flags, bool _enabled = true) final override;

        UpdateFlags             GetUpdateFlags          () const final override;
        void                    SetUpdateFlags          (UpdateFlags _flags, bool _enabled = true) final override;

        VG_INLINE Flags         getFlags                () const;
        VG_INLINE void          setFlags                (Flags _flags, bool _enabled);

        VG_INLINE UpdateFlags   getUpdateFlags          () const;
        VG_INLINE void          setUpdateFlags          (UpdateFlags _flags, bool _enabled = true);

        VG_INLINE GameObject *  getGameObject           () const;

    private:
        Flags                   m_flags;
        UpdateFlags             m_update;
    };
}

#if VG_ENABLE_INLINE
#include "Component.inl"
#endif