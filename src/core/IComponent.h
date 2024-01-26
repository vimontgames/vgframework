#pragma once

#include "core/Object/Object.h"
#include "core/Object/UpdateFlags.h"

namespace vg::core
{
    class IGameObject;

    class IComponent : public core::Object
    {
    public:
        VG_CLASS_DECL(IComponent, core::Object);

        enum class Flags : u32
        {
            Enabled     = 0x00000001,
            NoCulling   = 0x00000002
        };

        IComponent(const core::string & _name, IObject * _parent) :
            core::Object(_name, _parent)
        {

        }

        virtual void                FixedUpdate     (float _dt) = 0;
        virtual void                Update          (float _dt) = 0;
        virtual void                LateUpdate      (float _dt) = 0;

        virtual const IGameObject * GetGameObject   () const = 0;
        virtual IGameObject *       GetGameObject   () = 0;

        virtual Flags               GetFlags        () const = 0;
        virtual void                SetFlags        (Flags _flags, bool _enabled = true) = 0;

        virtual UpdateFlags         GetUpdateFlags  () const = 0;
        virtual void                SetUpdateFlags  (UpdateFlags _flags, bool _enabled = true) = 0;
    };
}