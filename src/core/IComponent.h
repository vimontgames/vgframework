#pragma once

#include "core/Object/Object.h"
#include "core/Object/Update.h"

namespace vg::core
{
    class IWorld;
    class IGameObject;
    class AABB;

    vg_enum_class(vg::core, ComponentFlags, u32,
        Enabled     = 0x00000001,
        NoCulling   = 0x00000002
    );

    class IComponent : public core::Object
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IComponent, core::Object);

        using Context = ComponentUpdateContext;

        virtual void                FixedUpdate             (const Context & _context) = 0;
        virtual void                Update                  (const Context & _context) = 0;
        virtual void                LateUpdate              (const Context & _context) = 0;

        virtual const IGameObject * GetGameObject           () const = 0;
        virtual IGameObject *       GetGameObject           () = 0;

        virtual void                Enable                  (bool _enable) = 0;

        virtual ComponentFlags      GetComponentFlags       () const = 0;
        virtual void                EnableComponentFlags    (ComponentFlags _flags, bool _enabled = true) = 0;

        virtual UpdateFlags         GetUpdateFlags          () const = 0;
        virtual void                EnableUpdateFlags       (UpdateFlags _flags, bool _enabled = true) = 0;

        virtual const string        GetSubObjectName        (uint _subObjectIndex) const = 0;

        virtual bool                TryGetAABB              (AABB & _aabb) const = 0;
    };
}