#include "physics/Precomp.h"
#include "character.h"
#include "physics/Shape/Shape.h"
#include "core/Math/Math.h"
#include "physics/Helper.h"
#include "physics/Shape/CapsuleShape.h"
#include "physics/Physics.h"

#include "CharacterDesc.hpp"

using namespace vg::core;

namespace vg::physics
{
    //--------------------------------------------------------------------------------------
    bool Character::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    Character::Character(const CharacterDesc * _characterDesc, Shape * _shape, const core::float4x4 & _world) :
        super(),
        m_shape(_shape),
        m_characterDesc(_characterDesc)
    {
  
    }

    //--------------------------------------------------------------------------------------
    Character::~Character()
    {

    }

    //--------------------------------------------------------------------------------------
    JPH::BodyInterface & Character::getBodyInterface() const
    {
        return Physics::get()->getPhysicsSystem()->GetBodyInterface();
    }

    //--------------------------------------------------------------------------------------
    VG_REGISTER_OBJECT_CLASS(RigidCharacter, "Rigid Character");
    //--------------------------------------------------------------------------------------
    bool RigidCharacter::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);
        return true;
    }

    //--------------------------------------------------------------------------------------
    RigidCharacter::RigidCharacter(const RigidCharacterDesc * _rigidCharacterDesc, Shape * _shape, const core::float4x4 & _world) :
        super((CharacterDesc*)_rigidCharacterDesc, _shape, _world)
    {
        JPH::Shape * joltShape = _shape->getJoltShape();
        VG_ASSERT(joltShape);

        // Get quaternion from matrix rotation part
        float4x4 world = _world;
        float4x4 rot = extractRotation(world);
        quaternion quat = getQuaternionFromRotationMatrix(rot);

        JPH::CharacterSettings settings;
        settings.mMaxSlopeAngle = _rigidCharacterDesc->m_maxSlopeAngle;
        settings.mLayer = getJoltObjectLayer(ObjectLayer::Moving);
        settings.mShape = _shape->getJoltShape();
        settings.mFriction = _rigidCharacterDesc->m_friction;
        settings.mMass = _rigidCharacterDesc->m_mass;

        // TODO : compute bounding "radius" from other primitives than capsule?
        CapsuleShape * capsuleShape = dynamic_cast<CapsuleShape *>(_shape);
        VG_ASSERT(capsuleShape);
        settings.mSupportingVolume = JPH::Plane(getJoltVec3(float3(0, 0, 1)), -capsuleShape->m_radius); // Accept contacts that touch the lower sphere of the capsule

        m_character = new JPH::Character(&settings, getJoltVec3(world[3].xyz), getJoltQuaternion(quat), 0, Physics::get()->getPhysicsSystem());
        m_character->AddRef();

        //getBodyInterface().SetFriction(m_character->GetBodyID(), _rigidCharacterDesc->m_friction);
        //getBodyInterface().SetRestitution(m_character->GetBodyID(), _rigidCharacterDesc->m_restitution);
        getBodyInterface().AddBody(m_character->GetBodyID(), JPH::EActivation::DontActivate);
    }

    //--------------------------------------------------------------------------------------
    RigidCharacter::~RigidCharacter()
    {
        if (m_character)
        {
            getBodyInterface().RemoveBody(m_character->GetBodyID());
            //getBodyInterface().DestroyBody(m_character->GetBodyID()); // Body will be destroyed by m_character ~ctor
        }
        VG_SAFE_RELEASE(m_character);
    }

    //--------------------------------------------------------------------------------------
    void RigidCharacter::Activate(const float4x4 & _world)
    {
        resetCharacter(_world);
        if (m_character)
            getBodyInterface().ActivateBody(m_character->GetBodyID());
    }

    //--------------------------------------------------------------------------------------
    void RigidCharacter::Deactivate(const float4x4 & _world)
    {
        if (m_character)
            getBodyInterface().DeactivateBody(m_character->GetBodyID());
        resetCharacter(_world);
    }

    //--------------------------------------------------------------------------------------
    void RigidCharacter::resetCharacter(const core::float4x4 & _world)
    {
        if (m_character)
        {
            float4x4 world = _world;
            float4x4 rot = extractRotation(_world);
            quaternion quat = getQuaternionFromRotationMatrix(rot);
            getBodyInterface().SetPositionRotationAndVelocity(m_character->GetBodyID(), getJoltVec3(world[3].xyz), getJoltQuaternion(quat), getJoltVec3(float3(0, 0, 0)), getJoltVec3(float3(0, 0, 0)));
        }
    }

    //--------------------------------------------------------------------------------------
    core::float3 RigidCharacter::GetVelocity() const
    {
        if (m_character)
            return fromJoltVec3(m_character->GetLinearVelocity());
        else
            return float3(0, 0, 0);
    }
    //--------------------------------------------------------------------------------------
    void RigidCharacter::SetVelocity(const core::float3 & _velocity)
    {
        if (m_character)
            m_character->SetLinearVelocity(getJoltVec3(_velocity));
    }

    //--------------------------------------------------------------------------------------
    float4x4 RigidCharacter::GetMatrix() const
    {
        float4x4 world = fromJoltMatrix(m_character->GetWorldTransform());
        return world;
    }
}