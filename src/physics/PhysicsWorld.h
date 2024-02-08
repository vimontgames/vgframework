#pragma once

#include "core/IWorld.h"

namespace vg::core
{
    class IWorld;
}

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

        void OnPlay() final override;

        VG_INLINE const vg::core::IWorld * getWorld() const
        {
            return m_world;
        }

        VG_INLINE JPH::PhysicsSystem * getPhysicsSystem() const
        {
            return m_physicsSystem;
        }

        VG_INLINE JPH::BodyInterface & getBodyInterface() const
        {
            return m_physicsSystem->GetBodyInterface();
        }

    private:
        const vg::core::IWorld *    m_world = nullptr;
        JPH::PhysicsSystem *        m_physicsSystem = nullptr;

    };
}