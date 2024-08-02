#pragma once

#include "Jolt/Physics/Collision/ContactListener.h"

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

    private:
        PhysicsWorld * m_physicsWorld = nullptr;
    };
}