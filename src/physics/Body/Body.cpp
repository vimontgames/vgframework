#include "physics/Precomp.h"
#include "Body.h"
#include "physics/Physics.h"
#include "physics/Helper.h"
#include "physics/Shape/Shape.h"
#include "core/Math/Math.h"
#include "Jolt/Physics/PhysicsSystem.h"
#include "Jolt/Physics/Body/BodyCreationSettings.h"

#include "physics/Body/BodyDesc.hpp"

using namespace vg::core;

namespace vg::physics
{
    //--------------------------------------------------------------------------------------
    bool Body::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    Body::Body(const BodyDesc * _bodyDesc, Shape * _shape, const core::float4x4 & _world) :
        super(),
        m_shape(_shape),
        m_bodyDesc(_bodyDesc)
    {
        JPH::Shape * joltShape = _shape->getJoltShape();
        VG_ASSERT(joltShape);
        
        // Get quaternion from matrix rotation part
        float4x4 world = _world;
        float4x4 rot = extractRotation(world);
        quaternion quat = getQuaternionFromRotationMatrix(rot);
        
        JPH::BodyCreationSettings bodySettings(joltShape, getJoltVec3(world[3].xyz), getJoltQuaternion(quat), getJoltMotionType(_bodyDesc->m_motion), getJoltObjectLayer(_bodyDesc->m_layer));
        
        if (_bodyDesc->m_overrideMass)
        {
            bodySettings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
            bodySettings.mMassPropertiesOverride.mMass = _bodyDesc->m_mass;
        }
        
        m_bodyID = getBodyInterface().CreateAndAddBody(bodySettings, JPH::EActivation::DontActivate);
        
        //getBodyInterface().SetFriction()
        //auto quality = bodyInterface.GetMotionQuality(bodyID);
        //bodyInterface.SetMotionQuality(bodyID, EMotionQuality::LinearCast);
    }

    //--------------------------------------------------------------------------------------
    Body::~Body()
    {
        if (!m_bodyID.IsInvalid())
        {
            getBodyInterface().RemoveBody(m_bodyID);
            getBodyInterface().DestroyBody(m_bodyID);
        }
    }

    //--------------------------------------------------------------------------------------
    void Body::Activate(const float4x4 & _world)
    {
        resetBody(_world);
        if (!m_bodyID.IsInvalid())
            getBodyInterface().ActivateBody(m_bodyID);
    }

    //--------------------------------------------------------------------------------------
    void Body::Deactivate(const float4x4 & _world)
    {
        getBodyInterface().DeactivateBody(m_bodyID);
        if (!m_bodyID.IsInvalid())
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
        float4x4 world = _world;
        float4x4 rot = extractRotation(_world);
        quaternion quat = getQuaternionFromRotationMatrix(rot);
        getBodyInterface().SetPositionRotationAndVelocity(m_bodyID, getJoltVec3(world[3].xyz), getJoltQuaternion(quat), getJoltVec3(float3(0, 0, 0)), getJoltVec3(float3(0, 0, 0)));
    }

    //--------------------------------------------------------------------------------------
    float4x4 Body::GetMatrix() const
    {
        JPH::BodyInterface & bodyInterface = Physics::get()->getPhysicsSystem()->GetBodyInterface();

        JPH::Vec3 position;
        JPH::Quat rotation;
        getBodyInterface().GetPositionAndRotation(m_bodyID, position, rotation);

        float4x4 world = float4x4(fromJoltQuaternion(rotation));
        world[3].xyz = fromJoltVec3(position);
        return world;
    }

    VG_REGISTER_OBJECT_CLASS(RigidBody, "RigidBody");

    //--------------------------------------------------------------------------------------
    bool RigidBody::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);
        return true;
    }

    //--------------------------------------------------------------------------------------
    RigidBody::RigidBody(const RigidBodyDesc * _bodyDesc, Shape * _shape, const core::float4x4 & _world) :
       super(_bodyDesc, _shape, _world)
    {

    }

    //--------------------------------------------------------------------------------------
    RigidBody::~RigidBody()
    {

    }
}