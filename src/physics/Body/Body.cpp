#include "physics/Precomp.h"
#include "Body.h"
#include "physics/Physics.h"
#include "physics/Helper.h"
#include "physics/Shape/Shape.h"
#include "core/Math/Math.h"
#include "physics/PhysicsWorld.h"

#include "physics/Body/BodyDesc.hpp"

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
    Body::Body(PhysicsWorld * _physicsWorld, const PhysicsBodyDesc * _bodyDesc, Shape * _shape, const core::float4x4 & _matrix) :
        super(),
        m_physicsWorld(_physicsWorld),
        m_shape(_shape),
        m_bodyDesc(_bodyDesc)
    {
        JPH::Shape * joltShape = _shape->getJoltShape();
        VG_ASSERT(joltShape);
        
        // Get quaternion from matrix rotation part
        float3x3 rot = extractRotation(_matrix);
        quaternion quat = quaternion(rot); 
        
        JPH::BodyCreationSettings bodySettings(joltShape, getJoltVec3(_matrix[3].xyz), getJoltQuaternion(quat), getJoltMotionType(_bodyDesc->m_motion), getJoltObjectLayer(_bodyDesc->m_layer));
        
        if (_bodyDesc->IsTrigger())
        {
            bodySettings.mIsSensor = true;
            bodySettings.mGravityFactor = 0;
            //bodySettings.mOverrideMassProperties = JPH::EOverrideMassProperties::MassAndInertiaProvided;
            //bodySettings.mMassPropertiesOverride.mMass = 0.0f;
            //bodySettings.mMassPropertiesOverride.mInertia = 0.0f;
        }
        else if (_bodyDesc->m_overrideMass)
        {
            bodySettings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
            bodySettings.mMassPropertiesOverride.mMass = _bodyDesc->m_mass;
        }
        
        m_bodyID = m_physicsWorld->getBodyInterface().CreateAndAddBody(bodySettings, JPH::EActivation::DontActivate);

        m_physicsWorld->getBodyInterface().SetFriction(m_bodyID, _bodyDesc->m_friction);
        m_physicsWorld->getBodyInterface().SetRestitution(m_bodyID, _bodyDesc->m_restitution);
    }

    //--------------------------------------------------------------------------------------
    Body::~Body()
    {
        if (!m_bodyID.IsInvalid())
        {
            m_physicsWorld->getBodyInterface().RemoveBody(m_bodyID);
            m_physicsWorld->getBodyInterface().DestroyBody(m_bodyID);
        }
    }

    //--------------------------------------------------------------------------------------
    void Body::Activate(const float4x4 & _world)
    {
        resetBody(_world);
        if (!m_bodyID.IsInvalid())
            m_physicsWorld->getBodyInterface().ActivateBody(m_bodyID);
    }

    //--------------------------------------------------------------------------------------
    void Body::Deactivate(const float4x4 & _world)
    {
        m_physicsWorld->getBodyInterface().DeactivateBody(m_bodyID);
        if (!m_bodyID.IsInvalid())
            resetBody(_world);
    }

    //--------------------------------------------------------------------------------------
    void Body::resetBody(const core::float4x4 & _world)
    {
        float3x3 rot = extractRotation(_world);
        quaternion quat = quaternion(rot);
        m_physicsWorld->getBodyInterface().SetPositionRotationAndVelocity(m_bodyID, getJoltVec3(_world[3].xyz), getJoltQuaternion(quat), getJoltVec3(float3(0, 0, 0)), getJoltVec3(float3(0, 0, 0)));
    }

    //--------------------------------------------------------------------------------------
    float4x4 Body::GetMatrix() const
    {
        JPH::Vec3 position;
        JPH::Quat rotation;
        m_physicsWorld->getBodyInterface().GetPositionAndRotation(m_bodyID, position, rotation);

        float4x4 world = float4x4(fromJoltQuaternion(rotation));
        world[3].xyz = fromJoltVec3(position);

        return world;
    }
}