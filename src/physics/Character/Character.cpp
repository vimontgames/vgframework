#include "physics/Precomp.h"
#include "Character.h"
#include "physics/Shape/Shape.h"
#include "core/Math/Math.h"
#include "physics/Shape/CapsuleShape.h"
#include "physics/Physics.h"
#include "physics/World/PhysicsWorld.h"

#pragma push_macro("new")
#undef new
#include "Jolt/Physics/Collision/ShapeCast.h"
#include "Jolt/Physics/Collision/CollisionCollectorImpl.h"
#include "Jolt/Physics/Collision/RayCast.h"
#pragma pop_macro("new")

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
    VG_REGISTER_CLASS(RigidCharacter, "Rigid Character");
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
            m_character->PostSimulation(getCharacterDesc()->m_maxSeparationDistance); 
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
    const core::IObject * RigidCharacter::GetGroundObject() const
    {
        return (const core::IObject *)m_character->GetGroundUserData();
    }

    //--------------------------------------------------------------------------------------
    class CustomBodyFilter : public JPH::BodyFilter
    {
    public:
        CustomBodyFilter(const JPH::BodyLockInterfaceLocking & _bodyLockInterface) :
            m_bodyLockInterface(_bodyLockInterface)
        {

        }

        bool ShouldCollide(const JPH::BodyID & _bodyID) const override
        {
            JPH::BodyLockRead lock(m_bodyLockInterface, _bodyID);
            if (lock.Succeeded())
            {
                const JPH::Body & body = lock.GetBody();
                return !body.IsSensor();
            }

            return false;
        }

    private:
        const JPH::BodyLockInterfaceLocking & m_bodyLockInterface;
    };

    //--------------------------------------------------------------------------------------
    bool RigidCharacter::CanTeleportTo(const core::float3 & _targetPos)
    {
        using namespace JPH;

        JPH::PhysicsSystem * physicsSystem = getPhysicsWorld()->getPhysicsSystem();

        // Get the shape and scale of the character
        const auto * shape = m_character->GetShape();
        const Vec3 shapeScale(1.0f, 1.0f, 1.0f);

        // Build a transform at the target position
        const Mat44 transform = Mat44::sTranslation(getJoltVec3(_targetPos));

        CollideShapeSettings settings;
        settings.mActiveEdgeMode = EActiveEdgeMode::CollideWithAll;
        settings.mBackFaceMode = EBackFaceMode::CollideWithBackFaces;
        settings.mCollectFacesMode = ECollectFacesMode::NoFaces;

        ClosestHitCollisionCollector<CollideShapeCollector> collector;  

        SpecifiedBroadPhaseLayerFilter broadphase_non_moving_filter(getJoltBroadPhaseLayer(physics::BPLayer::NonMoving));
        SpecifiedObjectLayerFilter object_non_moving_filter(getJoltObjectLayer(physics::ObjectLayer::NonMoving));

        CustomBodyFilter bodyFilter(physicsSystem->GetBodyLockInterface());

        // Check collision at target transform
        physicsSystem->GetNarrowPhaseQuery().CollideShape(
            shape,                          // inShape
            shapeScale,                     // inShapeScale
            transform,                      // inCenterOfMassTransform
            settings,                       // inCollideShapeSettings
            RVec3::sZero(),                 // inBaseOffset
            collector,                      // ioCollector
            {}, //broadphase_non_moving_filter,   // inBroadPhaseLayerFilter
            {}, //object_non_moving_filter,       // inObjectLayerFilter
            bodyFilter                      // inBodyFilter        
        );

        // If collector found anything, teleport is blocked
        return !collector.HadHit();
    }
}