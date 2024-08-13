#pragma once
#include "Physics_Consts.h"

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
    inline JPH::EMotionQuality getJoltMotionQuality(physics::MotionQuality _quality)
    {
        VG_STATIC_ASSERT(core::enumCount<JPH::EMotionQuality>()           == core::enumCount<physics::MotionQuality>(),           "Invalid enum size");
        VG_STATIC_ASSERT(core::asInteger(JPH::EMotionQuality::Discrete)   == core::asInteger(physics::MotionQuality::Discrete),   "Invalid enum value");
        VG_STATIC_ASSERT(core::asInteger(JPH::EMotionQuality::LinearCast) == core::asInteger(physics::MotionQuality::Continuous), "Invalid enum value");
        return (JPH::EMotionQuality)_quality;
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
    inline core::float4x4 fromJoltMatrix(const JPH::Mat44 & _mat)
    {
        JPH::Vec3 I = _mat.GetAxisX();
        JPH::Vec3 J = _mat.GetAxisY();
        JPH::Vec3 K = _mat.GetAxisZ();
        JPH::Vec3 T = _mat.GetTranslation();

        core::float4x4 matrix;
        matrix[0] = core::float4(I.GetX(), I.GetY(), I.GetZ(), 0.0f);
        matrix[1] = core::float4(J.GetX(), J.GetY(), J.GetZ(), 0.0f);
        matrix[2] = core::float4(K.GetX(), K.GetY(), K.GetZ(), 0.0f);
        matrix[3] = core::float4(T.GetX(), T.GetY(), T.GetZ(), 1.0f);
        return matrix;
    }

    //--------------------------------------------------------------------------------------
    inline core::float3 fromJoltVec3(const JPH::Vec3 & _vec)
    {
        return core::float3(_vec.GetX(), _vec.GetY(), _vec.GetZ());
    }
}