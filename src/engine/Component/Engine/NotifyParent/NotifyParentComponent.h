#pragma once

#include "core/Component/Behaviour/Behaviour.h"

vg_enum_class_ns(vg::engine, ParentNotificationTypeFlags, core::u32,
    CollisionEnter = 0x00000001,
    CollisionStay = 0x00000002,
    CollisionExit = 0x00000004,

    TriggerEnter = 0x00000010,
    TriggerStay = 0x00000020,
    TriggerExit = 0x00000040
);

namespace vg::engine
{
    class NotifyParentComponent final : public vg::core::Behaviour
    {
    public:
        VG_CLASS_DECL(NotifyParentComponent, vg::core::Behaviour);

        NotifyParentComponent(const core::string & _name, IObject * _parent);
        ~NotifyParentComponent();

        void    OnCollisionEnter(core::IGameObject * _other) final override;
        void    OnCollisionStay(core::IGameObject * _other) final override;
        void    OnCollisionExit(core::IGameObject * _other) final override;

        void    OnTriggerEnter(core::IGameObject * _other) final override;
        void    OnTriggerStay(core::IGameObject * _other) final override;
        void    OnTriggerExit(core::IGameObject * _other) final override;

    private:
        ParentNotificationTypeFlags m_notificationFlags = (ParentNotificationTypeFlags)0x0;
    };
}