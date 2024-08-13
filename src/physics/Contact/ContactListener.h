#pragma once

#include "Jolt/Physics/Collision/ContactListener.h"
#include "core/Container/AtomicVector.h"
#include "Contact.h"

namespace vg::physics
{
    class PhysicsWorld;

    class ContactListener : public JPH::ContactListener
    {
    public:
        ContactListener(PhysicsWorld * _physicsWorld);
        ~ContactListener();

        void OnContactAdded(const JPH::Body & _inBody1, const JPH::Body & _inBody2, const JPH::ContactManifold & _inManifold, JPH::ContactSettings & _ioSettings) final override;
        void OnContactPersisted(const JPH::Body & _inBody1, const JPH::Body & _inBody2, const JPH::ContactManifold & _inManifold, JPH::ContactSettings & _ioSettings) final override;
        void OnContactRemoved(const JPH::SubShapeIDPair & _inSubShapePair) final override;

        void clear();
        void update();

    private:
        PhysicsWorld *                  m_physicsWorld = nullptr;
        core::atomicvector<Contact>     m_triggerEnter;
        core::atomicvector<Contact>     m_triggerStay;
        core::atomicvector<Contact>     m_collisionEnter;
        core::atomicvector<Contact>     m_collisionStay;
    };
}