#include "PhysicsWorld.h"
#include "core/Timer/Timer.h"
#include "physics/Contact/ContactListener.h"

using namespace vg::core;

namespace vg::physics
{
    //--------------------------------------------------------------------------------------
    PhysicsWorld::PhysicsWorld(const core::IWorld * _world) :
        m_world(_world)
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

        m_contactListener = new ContactListener(this);
        m_physicsSystem->SetContactListener(m_contactListener);

        physics->registerPhysicsWorld(this);
    }

    //--------------------------------------------------------------------------------------
    PhysicsWorld::~PhysicsWorld()
    {
        m_physicsSystem->SetContactListener(nullptr);
        VG_SAFE_DELETE(m_contactListener);

        auto * physics = Physics::get();
        physics->unregisterPhysicsWorld(this);
        VG_SAFE_DELETE(m_physicsSystem);
    }

    //--------------------------------------------------------------------------------------
    void PhysicsWorld::OnPlay()
    {
        auto * physicsSystem = getPhysicsSystem();
        const auto startOptimizeBroadphase = Timer::getTick();
        physicsSystem->OptimizeBroadPhase();
        VG_INFO("[PhysicsWorld] Optimize BroadPhase in %.2f ms", Timer::getEnlapsedTime(startOptimizeBroadphase, Timer::getTick()));
    }
}