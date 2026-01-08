#include "physics/Precomp.h"
#include "Character.h"
#include "physics/Shape/Shape.h"
#include "core/Math/Math.h"
#include "physics/Shape/CapsuleShape.h"
#include "physics/Physics.h"
#include "physics/World/PhysicsWorld.h"

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
    Character::Character(PhysicsWorld * _physicsWorld, const CharacterDesc * _characterDesc, Shape * _shape, const core::float4x4 & _matrix, const core::string & _name, core::IObject * _parent) :
        super(_name, _parent),
        m_physicsWorld(_physicsWorld),
        m_shape(_shape),
        m_characterDesc(_characterDesc)
    {
  
    }

    //--------------------------------------------------------------------------------------
    Character::~Character()
    {

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
    RigidCharacter::RigidCharacter(PhysicsWorld * _physicsWorld, const RigidCharacterDesc * _rigidCharacterDesc, Shape * _shape, const core::float4x4 & _matrix, const core::string & _name, core::IObject * _parent) :
        super(_physicsWorld, (CharacterDesc*)_rigidCharacterDesc, _shape, _matrix, _name, _parent)
    {
        JPH::Shape * joltShape = _shape->getJoltShape();
        VG_ASSERT(joltShape);

        // Get quaternion from matrix rotation part
        float3x3 rot = extractRotation(_matrix);
        quaternion quat = quaternion(rot);

        JPH::CharacterSettings settings;
        settings.mMaxSlopeAngle = degreesToRadians(_rigidCharacterDesc->m_maxSlopeAngle);
        settings.mLayer = getJoltObjectLayer(ObjectLayer::Moving);
        settings.mShape = _shape->getJoltShape();
        settings.mFriction = _rigidCharacterDesc->m_friction;
        settings.mMass = _rigidCharacterDesc->m_mass;
        settings.mUp = getJoltVec3(float3(0, 0, 1));

        // TODO : compute bounding "radius" from other primitives than capsule?
        CapsuleShape * capsuleShape = dynamic_cast<CapsuleShape *>(_shape);
        VG_ASSERT(capsuleShape);
        settings.mSupportingVolume = JPH::Plane(getJoltVec3(float3(0, 0, 1)), -capsuleShape->m_bottomRadius); // Accept contacts that touch the lower sphere of the capsule

        #pragma push_macro("new")
        #undef new
        m_character = new JPH::Character(&settings, getJoltVec3(_matrix[3].xyz), getJoltQuaternion(quat), (u64)_parent, getPhysicsWorld()->getPhysicsSystem());
        #pragma pop_macro("new")

        m_character->AddRef();

        //getBodyInterface().SetFriction(m_character->GetBodyID(), _rigidCharacterDesc->m_friction);
        //getBodyInterface().SetRestitution(m_character->GetBodyID(), _rigidCharacterDesc->m_restitution);
        getPhysicsWorld()->getBodyInterface().AddBody(m_character->GetBodyID(), JPH::EActivation::DontActivate);
    }

    //--------------------------------------------------------------------------------------
    RigidCharacter::~RigidCharacter()
    {
        if (m_character)
            getPhysicsWorld()->getBodyInterface().RemoveBody(m_character->GetBodyID());

        VG_SAFE_RELEASE(m_character);
    }

    //--------------------------------------------------------------------------------------
    void RigidCharacter::Activate(const float4x4 & _world)
    {
        if (m_character)
        {
            getPhysicsWorld()->getBodyInterface().ActivateBody(m_character->GetBodyID());
            resetCharacter(_world);
        }
    }

    //--------------------------------------------------------------------------------------
    void RigidCharacter::Deactivate(const float4x4 & _world)
    {
        if (m_character)
        {
            resetCharacter(_world);
            getPhysicsWorld()->getBodyInterface().DeactivateBody(m_character->GetBodyID());
        }
    }

    //--------------------------------------------------------------------------------------
    void RigidCharacter::resetCharacter(const core::float4x4 & _world)
    {
        VG_ASSERT(m_character);

        float3x3 rot = extractRotation(_world);
        quaternion quat = quaternion(rot);
        getPhysicsWorld()->getBodyInterface().SetPositionRotationAndVelocity(m_character->GetBodyID(), getJoltVec3(_world[3].xyz), getJoltQuaternion(quat), getJoltVec3(float3(0, 0, 0)), getJoltVec3(float3(0, 0, 0)));
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
    void RigidCharacter::SetPosition(const core::float3 & _position)
    {
        if (m_character)
            m_character->SetPosition(getJoltVec3(_position));
    }

    //--------------------------------------------------------------------------------------
    void RigidCharacter::SetRotation(const core::quaternion & _rotation)
    {
        if (m_character)
            m_character->SetRotation(getJoltQuaternion(_rotation));
    }

    //--------------------------------------------------------------------------------------
    float4x4 RigidCharacter::GetMatrix() const
    {
        float4x4 world = fromJoltMatrix(m_character->GetWorldTransform());
        return world;
    }

    //--------------------------------------------------------------------------------------
    void RigidCharacter::FixedUpdate()
    {
    }

    //--------------------------------------------------------------------------------------
    void RigidCharacter::Update()
    {
        if (m_character)
            m_character->PostSimulation(0.00f);
    }

    //--------------------------------------------------------------------------------------
    GroundState RigidCharacter::GetGroundState() const
    {
        JPH::CharacterBase::EGroundState state = m_character->GetGroundState();
        switch (state)
        {
            default:
                VG_ASSERT_NOT_IMPLEMENTED();

            case JPH::CharacterBase::EGroundState::OnGround:
                return GroundState::Grounded;

            case JPH::CharacterBase::EGroundState::OnSteepGround:
                return GroundState::Blocked;

            case JPH::CharacterBase::EGroundState::NotSupported:
                return GroundState::Touching;

            case JPH::CharacterBase::EGroundState::InAir:
                return GroundState::InAir;
        }
    }

    //--------------------------------------------------------------------------------------
    bool RigidCharacter::CanMoveTo(const core::float3 & _position)
    {
        return false;
    }
}