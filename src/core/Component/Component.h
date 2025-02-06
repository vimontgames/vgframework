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

        void                        OnPlay                  () override;
        void                        OnStop                  () override;

        void                        FixedUpdate             (const Context & _context) override;
        void                        Update                  (const Context & _context) override;
        void                        LateUpdate              (const Context & _context) override;

        const IGameObject *         GetGameObject           () const final override;
        IGameObject *               GetGameObject           () final override;

        void                        SetPropertyValue        (const IProperty & _prop, void * _previousValue, void * _newValue) override;

        void                        Enable                  (bool _enable) final override;

        ComponentFlags              GetComponentFlags       () const final override;
        void                        EnableComponentFlags    (ComponentFlags _flags, bool _enabled = true) override;

        UpdateFlags                 GetUpdateFlags          () const final override;
        void                        EnableUpdateFlags       (UpdateFlags _flags, bool _enabled = true) final override;

        bool                        TryGetAABB              (AABB & _aabb) const override { return false; }

        const core::string          GetSubObjectName        (core::uint _subObjectIndex) const override;

        VG_INLINE ComponentFlags    getComponentFlags       () const;
        VG_INLINE void              setComponentFlags       (ComponentFlags _flags, bool _enabled);

        VG_INLINE UpdateFlags       getUpdateFlags          () const;
        VG_INLINE void              enableUpdateFlags       (UpdateFlags _flags, bool _enabled = true);

        VG_INLINE GameObject *      getGameObject           () const;

        VG_INLINE bool              isEnabled               () const;

    private:
        ComponentFlags              m_flags;
        UpdateFlags                 m_update;
    };
}

#if VG_ENABLE_INLINE
#include "Component.inl"
#endif