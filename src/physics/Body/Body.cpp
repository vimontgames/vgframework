#include "physics/Precomp.h"
#include "Body.h"
#include "physics/Physics.h"
#include "physics/Shape/Shape.h"
#include "core/Math/Math.h"
#include "physics/World/PhysicsWorld.h"

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
    Body::Body(PhysicsWorld * _physicsWorld, const PhysicsBodyDesc * _bodyDesc, Shape * _shape, const core::float4x4 & _matrix, const core::string & _name, core::IObject * _parent) :
        super(_name, _parent),
        m_physicsWorld(_physicsWorld),
        m_bodyDesc(_bodyDesc)
    {
        JPH::Shape * joltShape = _shape->getJoltShape();
        VG_ASSERT(joltShape);

        // Create body from shape directly
        createBodyFromJoltShape(_physicsWorld, _bodyDesc, joltShape, _matrix, _name, _parent);
    }

    //--------------------------------------------------------------------------------------
    Body::Body(PhysicsWorld * _physicsWorld, const PhysicsBodyDesc * _bodyDesc, const core::vector<ShapeInfo> & _shapes, const core::float4x4 & _matrix, const core::string & _name, core::IObject * _parent) :
        super(_name, _parent),
        m_physicsWorld(_physicsWorld),
        m_bodyDesc(_bodyDesc)
    {
        if (_shapes.size() > 0)
        {
            // Create body from a compound shape
            JPH::StaticCompoundShapeSettings compoundShapeSettings;

            for (uint i = 0; i < _shapes.size(); ++i)
            {
                auto * shape = (Shape *)_shapes[i].m_shape;
                VG_ASSERT(nullptr != shape);

                auto * joltShape = shape->getJoltShape();
                VG_ASSERT(nullptr != joltShape);
                if (joltShape)
                    compoundShapeSettings.AddShape(getJoltVec3(_shapes[i].m_translation), getJoltQuaternion(_shapes[i].m_rotation), shape->getJoltShape());
            }

            JPH::Ref<JPH::Shape> compoundShape = compoundShapeSettings.Create().Get();

            createBodyFromJoltShape(_physicsWorld, _bodyDesc, compoundShape, _matrix, _name, _parent);
        }
        else if (_shapes.size() == 1)
        {
            // Create body from shape directly if there's only one shape
            JPH::Shape * joltShape = ((Shape*)_shapes[0].m_shape)->getJoltShape();
            VG_ASSERT(joltShape);

            // Create body from shape directly
            createBodyFromJoltShape(_physicsWorld, _bodyDesc, joltShape, _matrix, _name, _parent);
        }
    }

    //--------------------------------------------------------------------------------------
    void Body::createBodyFromJoltShape(PhysicsWorld * _physicsWorld, const PhysicsBodyDesc * _bodyDesc, JPH::Shape * _joltShape, const core::float4x4 & _matrix, const core::string & _name, core::IObject * _parent)
    {
        // Get quaternion from matrix rotation part
        float3x3 rot = extractRotation(_matrix);
        quaternion quat = quaternion(rot);

        JPH::BodyCreationSettings bodySettings(_joltShape, getJoltVec3(_matrix[3].xyz), getJoltQuaternion(quat), getJoltMotionType(_bodyDesc->m_motion), getJoltObjectLayer(_bodyDesc->m_layer));

        bodySettings.mUserData = (u64)_parent;

        if (_bodyDesc->IsTrigger())
        {
            bodySettings.mIsSensor = true;
            bodySettings.mGravityFactor = 0;
        }

        bool overrideMass = _bodyDesc->m_overrideMass;
        bool overrideCOM = _bodyDesc->m_overrideCenterOfMassOffset;

        if (overrideMass || overrideCOM)
        {
            JPH::MassProperties mp = _joltShape->GetMassProperties();

            // COM offset
            if (overrideCOM)
                mp.Translate(getJoltVec3(_bodyDesc->GetCenterOfMassOffset()));

            if (overrideMass)
                mp.ScaleToMass(_bodyDesc->m_mass);

            bodySettings.mMassPropertiesOverride = mp;
            bodySettings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
        }

        bodySettings.mMotionQuality = getJoltMotionQuality(_bodyDesc->m_motionQuality);

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

    //--------------------------------------------------------------------------------------
    void Body::SetMatrix(core::float4x4 _world)
    {
        float3x3 rot = extractRotation(_world);
        quaternion quat = quaternion(rot);
        m_physicsWorld->getBodyInterface().SetPositionAndRotation(m_bodyID, getJoltVec3(_world[3].xyz), getJoltQuaternion(quat), JPH::EActivation::Activate);
    }

    //--------------------------------------------------------------------------------------
    void Body::AddImpulse(const core::float3 & _impulse)
    {
        const float mass = m_bodyDesc->GetMass();
        VG_ASSERT(mass > 0.0f);
        m_physicsWorld->getBodyInterface().AddImpulse(m_bodyID, getJoltVec3(_impulse) / mass);
    }

    //--------------------------------------------------------------------------------------
    void Body::SetLinearVelocity(const core::float3 & _velocity)
    {
        m_physicsWorld->getBodyInterface().SetLinearVelocity(m_bodyID, getJoltVec3(_velocity));
    }

    //--------------------------------------------------------------------------------------
    core::float3 Body::GetLinearVelocity() const
    {
        return fromJoltVec3(m_physicsWorld->getBodyInterface().GetLinearVelocity(m_bodyID));
    }

    //--------------------------------------------------------------------------------------
    void Body::SetAngularVelocity(const core::float3 & _velocity)
    {
        m_physicsWorld->getBodyInterface().SetAngularVelocity(m_bodyID, getJoltVec3(_velocity));
    }

    //--------------------------------------------------------------------------------------
    core::float3 Body::GetAngularVelocity() const
    {
        return fromJoltVec3(m_physicsWorld->getBodyInterface().GetAngularVelocity(m_bodyID));
    }
}