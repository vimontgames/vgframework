#include "PhysicsWorld.h"

namespace vg::physics
{
    //--------------------------------------------------------------------------------------
    PhysicsWorld::PhysicsWorld(const core::IWorld * _world)
    {
        setName(_world->getName());

        #pragma push_macro("new")
        #undef new
        m_physicsSystem = new JPH::PhysicsSystem();
        #pragma pop_macro("new") 

        auto * physics = Physics::get();
        const auto & params = physics->getPhysicsCreationParams();

        m_physicsSystem->Init(params.maxBodies, params.numBodyMutexes, params.maxBodyPairs, params.maxContactConstraints, physics->getBroadPhaseLayer(), physics->getBroadPhaseFilter(), physics->getObjectfilter());
        m_physicsSystem->SetGravity(JPH::Vec3(0, 0, -9.81f));

        physics->registerPhysicsWorld(this);
    }

    //--------------------------------------------------------------------------------------
    PhysicsWorld::~PhysicsWorld()
    {
        auto * physics = Physics::get();
        physics->unregisterPhysicsWorld(this);
        VG_SAFE_DELETE(m_physicsSystem);
    }
}