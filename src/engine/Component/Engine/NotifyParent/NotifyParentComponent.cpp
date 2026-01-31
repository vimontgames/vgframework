#include "engine/Precomp.h"
#include "NotifyParentComponent.h"
#include "editor/Editor_Consts.h"
#include "core/IGameObject.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_COMPONENT_CLASS(NotifyParentComponent, "Notify Parent", "Engine", "This component will propagate selected engine callbacks to parent GameObject", editor::style::icon::Parent, getPriority(ComponentGroup::Behaviour));

    //--------------------------------------------------------------------------------------
    bool NotifyParentComponent::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyEnumBitfield(NotifyParentComponent, ParentNotificationTypeFlags, m_notificationFlags, "Notifications");
        setPropertyDescription(NotifyParentComponent, m_notificationFlags, "Select the engine callbacks to propagate to parent GameObject");

        return true;
    }

    //--------------------------------------------------------------------------------------
    NotifyParentComponent::NotifyParentComponent(const core::string & _name, IObject * _parent) :
        super(_name, _parent)
    {
        EnableUpdateFlags(UpdateFlags::Update, false);
    }

    //--------------------------------------------------------------------------------------
    NotifyParentComponent::~NotifyParentComponent()
    {
     
    }

    //--------------------------------------------------------------------------------------
    void NotifyParentComponent::OnCollisionEnter(IGameObject * _other)
    {
        if (asBool(ParentNotificationTypeFlags::CollisionEnter & m_notificationFlags))
        {
            if (IGameObject * parent = VG_SAFE_STATIC_CAST(IGameObject, GetParent()))
                parent->OnCollisionEnter(_other);
        }
    }

    //--------------------------------------------------------------------------------------
    void NotifyParentComponent::OnCollisionStay(IGameObject * _other)
    {
        if (asBool(ParentNotificationTypeFlags::CollisionStay & m_notificationFlags))
        {
            if (IGameObject * parent = VG_SAFE_STATIC_CAST(IGameObject, GetParent()))
                parent->OnCollisionStay(_other);
        }
    }

    //--------------------------------------------------------------------------------------
    void NotifyParentComponent::OnCollisionExit(IGameObject * _other)
    {
        if (asBool(ParentNotificationTypeFlags::CollisionExit & m_notificationFlags))
        {
            if (IGameObject * parent = VG_SAFE_STATIC_CAST(IGameObject, GetParent()))
                parent->OnCollisionExit(_other);
        }
    }

    //--------------------------------------------------------------------------------------
    void NotifyParentComponent::OnTriggerEnter(IGameObject * _other)
    {
        if (asBool(ParentNotificationTypeFlags::TriggerEnter & m_notificationFlags))
        {
            if (IGameObject * parent = VG_SAFE_STATIC_CAST(IGameObject, GetGameObject()->getParent()))
                parent->OnTriggerEnter(_other);
        }
    }

    //--------------------------------------------------------------------------------------
    void NotifyParentComponent::OnTriggerStay(IGameObject * _other)
    {
        if (asBool(ParentNotificationTypeFlags::TriggerStay & m_notificationFlags))
        {
            if (IGameObject * parent = VG_SAFE_STATIC_CAST(IGameObject, GetParent()))
                parent->OnTriggerStay(_other);
        }
    }

    //--------------------------------------------------------------------------------------
    void NotifyParentComponent::OnTriggerExit(IGameObject * _other)
    {
        if (asBool(ParentNotificationTypeFlags::TriggerExit & m_notificationFlags))
        {
            if (IGameObject * parent = VG_SAFE_STATIC_CAST(IGameObject, GetParent()))
                parent->OnTriggerExit(_other);
        }

    }
}