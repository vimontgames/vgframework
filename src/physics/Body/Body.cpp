#include "physics/Precomp.h"
#include "Body.h"
#include "physics/Physics.h"
#include "physics/Helper.h"
#include "core/Math/Math.h"
#include "Jolt/Physics/PhysicsSystem.h"

using namespace vg::core;

namespace vg::physics
{
    VG_REGISTER_OBJECT_CLASS(Body, "Body");

    //--------------------------------------------------------------------------------------
    bool Body::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    Body::Body(const string & _name, IObject * _parent) :
        super(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    Body::Body(const JPH::BodyID _joltBodyID, const core::float4x4 & _transform) :
        super(),
        m_transform(_transform)
    {
        VG_ASSERT(m_joltBodyID.IsInvalid());
        m_joltBodyID = _joltBodyID;
    }

    //--------------------------------------------------------------------------------------
    Body::~Body()
    {
        getBodyInterface().RemoveBody(m_joltBodyID);
        getBodyInterface().DestroyBody(m_joltBodyID);
    }

    //--------------------------------------------------------------------------------------
    void Body::Activate(const float4x4 & _world)
    {
        resetBody(_world);
        getBodyInterface().ActivateBody(m_joltBodyID);
    }

    //--------------------------------------------------------------------------------------
    void Body::Deactivate(const float4x4 & _world)
    {
        getBodyInterface().DeactivateBody(m_joltBodyID);
        resetBody(_world);
    }

    //--------------------------------------------------------------------------------------
    JPH::BodyInterface & Body::getBodyInterface() const
    {
        return Physics::get()->getPhysicsSystem()->GetBodyInterface();
    }

    //--------------------------------------------------------------------------------------
    void Body::resetBody(const core::float4x4 & _world)
    {
        float4x4 world = mul(m_transform, _world);
        float4x4 rot = extractRotation(_world);
        quaternion quat = getQuaternionFromRotationMatrix(rot);
        getBodyInterface().SetPositionRotationAndVelocity(m_joltBodyID, getJoltVec3(world[3].xyz), getJoltQuaternion(quat), getJoltVec3(float3(0, 0, 0)), getJoltVec3(float3(0, 0, 0)));
    }

    //--------------------------------------------------------------------------------------
    float4x4 Body::GetMatrix() const
    {
        JPH::BodyInterface & bodyInterface = Physics::get()->getPhysicsSystem()->GetBodyInterface();
        
        JPH::Vec3 position;
        JPH::Quat rotation;
        getBodyInterface().GetPositionAndRotation(m_joltBodyID, position, rotation);

        float4x4 world = float4x4(fromJoltQuaternion(rotation));
        world[3].xyz = fromJoltVec3(position);
        world = mul(world, inverse(m_transform)); // TODO : store inverse transform?
        return world;
    }
}