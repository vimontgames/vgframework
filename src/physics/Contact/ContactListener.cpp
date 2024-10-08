#include "physics/Precomp.h"
#include "ContactListener.h"
#include "core/IGameObject.h"
#include "physics/World/PhysicsWorld.h"
#include "Jolt/Physics/Body/Body.h"
#include "physics/Physics.h"

using namespace vg::core;

namespace vg::physics
{
    static const uint maxTriggerContactPerFrame = 1024;
    static const uint maxCollisionContactPerFrame = 1024;

    //--------------------------------------------------------------------------------------
    ContactListener::ContactListener(PhysicsWorld * _physicsWorld) :
        m_physicsWorld(_physicsWorld),
        m_triggerEnter(maxTriggerContactPerFrame),
        m_triggerStay(maxTriggerContactPerFrame),
        m_collisionEnter(maxCollisionContactPerFrame),
        m_collisionStay(maxCollisionContactPerFrame)
    {

    }

    //--------------------------------------------------------------------------------------
    ContactListener::~ContactListener()
    {

    }

    //--------------------------------------------------------------------------------------
    // Called after detecting a collision between a body pair, but before calling OnContactAdded and before adding the contact constraint. 
    // If the function rejects the contact, the contact will not be added and any other contacts between this body pair will not be processed. 
    // This function will only be called once per PhysicsSystem::Update per body pair and may not be called again the next update if a contact 
    // persists and no new contact pairs between sub shapes are found. 
    // This is a rather expensive time to reject a contact point since a lot of the collision detection has happened already, make sure you 
    // filter out the majority of undesired body pairs through the ObjectLayerPairFilter that is registered on the PhysicsSystem. Note that this 
    // callback is called when all bodies are locked, so don't use any locking functions! Body 1 will have a motion type that is larger or equal 
    // than body 2's motion type (order from large to small: dynamic -> kinematic -> static). When motion types are equal, they are ordered by 
    // BodyID. The collision result (inCollisionResult) is reported relative to inBaseOffset.
    //--------------------------------------------------------------------------------------
    JPH::ValidateResult ContactListener::OnContactValidate(const JPH::Body & _inBody1, const JPH::Body & _inBody2, JPH::RVec3Arg _inBaseOffset, const JPH::CollideShapeResult & _inCollisionResult)
    {
        auto * obj1 = (core::IObject *)_inBody1.GetUserData();
        auto * obj2 = (core::IObject *)_inBody2.GetUserData();

        auto & callbacks = Physics::get()->getCallbacks();
        if (nullptr != callbacks.validateContact)
            return callbacks.validateContact(obj1, obj2) ? JPH::ValidateResult::AcceptAllContactsForThisBodyPair : JPH::ValidateResult::RejectAllContactsForThisBodyPair;
        else
            return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
    }

    //--------------------------------------------------------------------------------------
    // Handle contact
    //--------------------------------------------------------------------------------------
    void ContactListener::OnContactAdded(const JPH::Body & _inBody1, const JPH::Body & _inBody2, const JPH::ContactManifold & _inManifold, JPH::ContactSettings & _ioSettings)
    {
        auto * obj1 = (core::IObject *)_inBody1.GetUserData();
        auto * obj2 = (core::IObject *)_inBody2.GetUserData();

        if (_ioSettings.mIsSensor)
        {
            if (_inBody1.IsSensor())
                m_triggerEnter.push_back_atomic(Contact(VG_SAFE_STATIC_CAST(core::IGameObject, obj1->GetParent()), VG_SAFE_STATIC_CAST(core::IGameObject, obj2->GetParent())));

            if (_inBody2.IsSensor())
                m_triggerEnter.push_back_atomic(Contact(VG_SAFE_STATIC_CAST(core::IGameObject, obj2->GetParent()), VG_SAFE_STATIC_CAST(core::IGameObject, obj1->GetParent())));
        }
        else
        {
            m_collisionEnter.push_back_atomic(Contact(VG_SAFE_STATIC_CAST(core::IGameObject, obj1->GetParent()), VG_SAFE_STATIC_CAST(core::IGameObject, obj2->GetParent())));
            m_collisionEnter.push_back_atomic(Contact(VG_SAFE_STATIC_CAST(core::IGameObject, obj2->GetParent()), VG_SAFE_STATIC_CAST(core::IGameObject, obj1->GetParent())));
        }

        //VG_INFO("[Physics] Contact between \"%s\" and \"%s\"", obj1->GetParentGameObject()->getName().c_str(), obj2->GetParentGameObject()->getName().c_str());
    }

    //--------------------------------------------------------------------------------------
    // Handle persistent contact
    //--------------------------------------------------------------------------------------
    void ContactListener::OnContactPersisted(const JPH::Body & _inBody1, const JPH::Body & _inBody2, const JPH::ContactManifold & _inManifold, JPH::ContactSettings & _ioSettings)
    {
        auto * obj1 = (core::IObject *)_inBody1.GetUserData();
        auto * obj2 = (core::IObject *)_inBody2.GetUserData();

        if (_ioSettings.mIsSensor)
        {
            m_triggerStay.push_back_atomic(Contact(VG_SAFE_STATIC_CAST(core::IGameObject, obj1->GetParent()), VG_SAFE_STATIC_CAST(core::IGameObject, obj2->GetParent())));
        }
        //else
        //{
        //    m_collisionStay.push_back_atomic(Contact(VG_SAFE_STATIC_CAST(core::IGameObject, obj1->getParent()), VG_SAFE_STATIC_CAST(core::IGameObject, obj2->getParent())));
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

    //--------------------------------------------------------------------------------------
    void ContactListener::clear()
    {
        VG_PROFILE_CPU("Clear");

        m_triggerEnter.clear();
        m_triggerStay.clear();
        m_collisionEnter.clear();
        m_collisionStay.clear();
    }

    //--------------------------------------------------------------------------------------
    void ContactListener::update()
    {
        VG_PROFILE_CPU("ContactListener");

        for (uint i = 0; i < m_triggerEnter.size(); ++i)
        {
            const auto & contact = m_triggerEnter[i];
            contact.m_obj1->OnTriggerEnter(contact.m_obj2);
        }

        for (uint i = 0; i < m_triggerStay.size(); ++i)
        {
            const auto & contact = m_triggerStay[i];
            contact.m_obj1->OnTriggerStay(contact.m_obj2);
        }

        for (uint i = 0; i < m_collisionEnter.size(); ++i)
        {
            const auto & contact = m_collisionEnter[i];
            contact.m_obj1->OnCollisionEnter(contact.m_obj2);
        }

        for (uint i = 0; i < m_collisionStay.size(); ++i)
        {
            const auto & contact = m_collisionStay[i];
            contact.m_obj1->OnCollisionStay(contact.m_obj2);
        }
    }
}