#pragma once

#include "core/IWorld.h"

namespace JPH
{
    class PhysicsSystem;
}

namespace vg::physics
{
    //--------------------------------------------------------------------------------------
    class PhysicsWorld : public core::IPhysicsWorld
    {
    public:
        PhysicsWorld(const core::IWorld * _world);
        ~PhysicsWorld();

        VG_INLINE JPH::PhysicsSystem * getPhysicsSystem() const
        {
            return m_physicsSystem;
        }

        VG_INLINE JPH::BodyInterface & getBodyInterface() const
        {
            return m_physicsSystem->GetBodyInterface();
        }

    private:
        JPH::PhysicsSystem * m_physicsSystem = nullptr;
    };
}