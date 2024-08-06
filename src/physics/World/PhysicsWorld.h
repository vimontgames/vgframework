#pragma once

#include "core/IWorld.h"

namespace vg::core
{
    class IWorld;
}

namespace JPH
{
    class PhysicsSystem;
    class TempAllocatorMalloc;
    class JobSystem;
}

namespace vg::physics
{
    class ContactListener;
    class JobSystemAdapter;

    class PhysicsWorld : public core::IPhysicsWorld
    {
    public:
        PhysicsWorld(const core::IWorld * _world);
        ~PhysicsWorld();

        void                                OnPlay              () final override;
        void                                Update              (float _dt, JPH::TempAllocatorMalloc & _tempAllocator, JPH::JobSystem * _jobSystem);

        VG_INLINE const vg::core::IWorld *  getWorld            () const;
        VG_INLINE JPH::PhysicsSystem *      getPhysicsSystem    () const;
        VG_INLINE JPH::BodyInterface &      getBodyInterface    () const;

    private:
        const vg::core::IWorld *            m_world = nullptr;
        JPH::PhysicsSystem *                m_physicsSystem = nullptr;
        ContactListener *                   m_contactListener = nullptr;
    };
}

#if VG_ENABLE_INLINE
#include "PhysicsWorld.inl"
#endif