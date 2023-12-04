#pragma once
#include "Physics_Consts.h"
#include "Jolt/Physics/Body/MotionType.h"

namespace vg::physics
{
    //--------------------------------------------------------------------------------------
    inline JPH::BroadPhaseLayer getJoltBroadPhaseLayer(physics::BPLayer _layer)
    {
        return (JPH::BroadPhaseLayer)(JPH::BroadPhaseLayer::Type)_layer;
    }

    //--------------------------------------------------------------------------------------
    inline JPH::EMotionType getJoltMotionType(physics::MotionType _motion)
    {
        return (JPH::EMotionType)_motion;
    }

    //--------------------------------------------------------------------------------------
    inline JPH::ObjectLayer getJoltObjectLayer(physics::ObjectLayer _layer)
    {
        return (JPH::ObjectLayer)_layer;
    }

    //--------------------------------------------------------------------------------------
    inline JPH::Vec3 getJoltVec3(const core::float3 & _vec)
    {
        return JPH::Vec3Arg(_vec.x, _vec.y, _vec.z);
    }

    //--------------------------------------------------------------------------------------
    inline JPH::Quat getJoltQuaternion(const core::quaternion & _quat)
    {
        return JPH::Quat(_quat.x, _quat.y, _quat.z, _quat.w);
    }

    //--------------------------------------------------------------------------------------
    inline core::quaternion fromJoltQuaternion(const JPH::Quat & _quat)
    {
        return core::quaternion(_quat.GetX(), _quat.GetY(), _quat.GetZ(), _quat.GetW());
    }

    //--------------------------------------------------------------------------------------
    inline core::float3 fromJoltVec3(const JPH::Vec3 & _vec)
    {
        return core::float3(_vec.GetX(), _vec.GetY(), _vec.GetZ());
    }
}