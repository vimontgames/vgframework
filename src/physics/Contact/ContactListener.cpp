#include "physics/Precomp.h"
#include "ContactListener.h"
#include "core/IGameObject.h"
#include "physics/PhysicsWorld.h"
#include "Jolt/Physics/Body/Body.h"

namespace vg::physics
{
    //--------------------------------------------------------------------------------------
    ContactListener::ContactListener(PhysicsWorld * _physicsWorld) :
        m_physicsWorld(_physicsWorld)
    {

    }

    //--------------------------------------------------------------------------------------
    ContactListener::~ContactListener()
    {

    }

    //--------------------------------------------------------------------------------------
    // Handle contact
    //--------------------------------------------------------------------------------------
    void ContactListener::OnContactAdded(const JPH::Body & _inBody1, const JPH::Body & _inBody2, const JPH::ContactManifold & _inManifold, JPH::ContactSettings & _ioSettings)
    {
        if (_ioSettings.mIsSensor)
        {
            const auto * obj1 = (const core::IObject *)_inBody1.GetUserData();
            const auto * obj2 = (const core::IObject *)_inBody2.GetUserData();

            VG_INFO("[Physics] Trigger contact added between \"%s\" and \"%s\"", obj1->GetParentGameObject()->getName().c_str(), obj2->GetParentGameObject()->getName().c_str());
        }
    }

    //--------------------------------------------------------------------------------------
    // Handle persistent contact
    //--------------------------------------------------------------------------------------
    void ContactListener::OnContactPersisted(const JPH::Body & _inBody1, const JPH::Body & _inBody2, const JPH::ContactManifold & _inManifold, JPH::ContactSettings & _ioSettings)
    {
        //if (_inBody1.IsSensor() || _inBody2.IsSensor())
        //{
        //    const auto * obj1 = (const core::IObject *)_inBody1.GetUserData();
        //    const auto * obj2 = (const core::IObject *)_inBody2.GetUserData();
        //
        //    VG_INFO("[Physics] Trigger contact persisted between \"%s\" and \"%s\"", obj1->GetParentGameObject()->getName().c_str(), obj2->GetParentGameObject()->getName().c_str());
        //}
    }

    //--------------------------------------------------------------------------------------
    // Handle contact removal
    //--------------------------------------------------------------------------------------
    void ContactListener::OnContactRemoved(const JPH::SubShapeIDPair & _inSubShapePair)
    {
        //if (_inSubShapePair.GetBody1ID())
        //{
        //    auto & bodyLockInterface = m_physicsWorld->getPhysicsSystem()->GetBodyLockInterface();
        //
        //    const core::IObject * obj1 = nullptr;
        //    JPH::BodyLockRead lock1(bodyLockInterface, _inSubShapePair.GetBody1ID());
        //    if (lock1.Succeeded())
        //    {
        //        const JPH::Body & body1 = lock1.GetBody();
        //        obj1 = (const core::IObject *)body1.GetUserData();
        //    }
        //
        //    const core::IObject * obj2 = nullptr;
        //    JPH::BodyLockRead lock2(bodyLockInterface, _inSubShapePair.GetBody2ID());
        //    if (lock2.Succeeded())
        //    {
        //        const JPH::Body & body2 = lock2.GetBody();
        //        obj2 = (const core::IObject *)body2.GetUserData();
        //    }
        //
        //    VG_INFO("[Physics] Trigger contact removed between \"%s\" and \"%s\"", obj1->GetParentGameObject()->getName().c_str(), obj2->GetParentGameObject()->getName().c_str());
        //}
    }
}