#pragma once

#include "core/Object/Object.h"
#include "core/Object/UpdateFlags.h"

namespace vg::core
{
    class IGameObject;

     enum class ComponentFlags : u32
    {
        Enabled     = 0x00000001,
        NoCulling   = 0x00000002
    };

    class IComponent : public core::Object
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IComponent, core::Object);

        virtual void                    FixedUpdate         (float _dt) = 0;
        virtual void                    Update              (float _dt) = 0;
        virtual void                    LateUpdate          (float _dt) = 0;

        virtual const IGameObject *     GetGameObject       () const = 0;
        virtual IGameObject *           GetGameObject       () = 0;

        virtual ComponentFlags          GetComponentFlags   () const = 0;
        virtual void                    SetComponentFlags   (ComponentFlags _flags, bool _enabled = true) = 0;

        virtual UpdateFlags             GetUpdateFlags      () const = 0;
        virtual void                    SetUpdateFlags      (UpdateFlags _flags, bool _enabled = true) = 0;

        virtual const core::string      GetSubObjectName    (core::uint _subObjectIndex) const = 0;
    };
}