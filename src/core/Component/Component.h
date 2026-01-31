#pragma once

#include "core/IComponent.h"

namespace vg::core
{
    class GameObject;

    //--------------------------------------------------------------------------------------
    // Convert component priority to a single u16 value for sorting
    //--------------------------------------------------------------------------------------
    vg_enum_class(vg::core, ComponentGroup, core::u16,
        Behaviour = 0,
        Physics,
        Animation,
        Render
    );

    vg_enum_class(vg::core, ComponentPriority, core::u16,
        Early = 0,
        Default,
        Late
    );

    vg_enum_class(vg::core, ComponentMultithreadType, core::u16,
        Serial = 0,
        Job
    );

    inline u16 getPriority(ComponentGroup _group, ComponentPriority _priority = ComponentPriority::Default, ComponentMultithreadType _multithreadType = ComponentMultithreadType::Serial)
    {
        return (u16(_group) << 3) | u16(_priority)<<1 | u16(_multithreadType);
    }

    inline ComponentGroup getComponentGroup(u16 _priority)
    {
        return (ComponentGroup)(_priority >> 3);
    }

    inline ComponentPriority getComponentPriority(u16 _priority)
    {
        return (ComponentPriority)((_priority>>1) & 0x3);
    }

    inline ComponentMultithreadType getComponentMultithreadType(u16 _priority)
    {
        return (ComponentMultithreadType)(_priority & 1);
    }

    // Random thoughts ...
    // - Instead of a list of {GameObject, Component} shall we gather a list of {GameObject(1), Components(N)}? 
    // - Is it possible to register Components to update the way we do for graphic instances?
    // - Just store a list of enabled GameObjects to split between jobs and skip components not to update? GameObject + component indices? Just GO = slower in // jobs but faster gather

    class Component : public IComponent
    {
    public:
        VG_CLASS_DECL(Component, IComponent);

                                    Component               (const core::string & _name, IObject * _parent);
                                    ~Component              ();

        void                        OnPlay                  () override;
        void                        OnStop                  () override;

        void                        OnEnable                () override;
        void                        OnDisable               () override;

        void                        FixedUpdate             (const Context & _context) override;
        void                        Update                  (const Context & _context) override;
        void                        LateUpdate              (const Context & _context) override;
        void                        ToolUpdate              (const Context & _context) override;

        const IGameObject *         GetGameObject           () const final override;
        IGameObject *               GetGameObject           () final override;
        bool                        UpdateFlagsFromGameObject() override { return false; }

        void                        OnPropertyChanged       (core::IObject * _object, const core::IProperty & _prop, bool _notifyParent) override;
        void                        SetPropertyValue        (const IProperty & _prop, void * _previousValue, void * _newValue) override;

        void                        Enable                  (bool _enable) final override;

        ComponentFlags              GetComponentFlags       () const final override;
        void                        EnableComponentFlags    (ComponentFlags _flags, bool _enabled = true) override;

        UpdateFlags                 GetUpdateFlags          () const final override;
        void                        SetUpdateFlags          (UpdateFlags _flags) final override;
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