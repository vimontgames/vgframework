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

        void                        FixedUpdate             (float _dt) override;
        void                        Update                  (float _dt) override;
        void                        LateUpdate              (float _dt) override;

        const IGameObject *         GetGameObject           () const final override;
        IGameObject *               GetGameObject           () final override;

        ComponentFlags              GetComponentFlags       () const final override;
        void                        SetComponentFlags       (ComponentFlags _flags, bool _enabled = true) override;

        UpdateFlags                 GetUpdateFlags          () const final override;
        void                        SetUpdateFlags          (UpdateFlags _flags, bool _enabled = true) final override;

        const core::string          GetSubObjectName        (core::uint _subObjectIndex) const override;

        VG_INLINE ComponentFlags    getComponentFlags       () const;
        VG_INLINE void              setComponentFlags       (ComponentFlags _flags, bool _enabled);

        VG_INLINE UpdateFlags       getUpdateFlags          () const;
        VG_INLINE void              setUpdateFlags          (UpdateFlags _flags, bool _enabled = true);

        VG_INLINE GameObject *      getGameObject           () const;

    private:
        ComponentFlags              m_flags;
        UpdateFlags                 m_update;
    };
}

#if VG_ENABLE_INLINE
#include "Component.inl"
#endif