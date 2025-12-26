#pragma once

#include "physics/IBody.h"

namespace vg::core
{
    class IWorld;
}

namespace JPH
{
    class BodyInterface;
}

namespace vg::physics
{
    class PhysicsBodyDesc;
    class Shape;
    class PhysicsWorld;

    class Body final : public IBody
    {
    public:
        VG_CLASS_DECL(Body, IBody);
        VG_CLASS_CTOR_HEADER_IMPL(Body, IBody);

        Body(PhysicsWorld * _physicsWorld, const PhysicsBodyDesc * _bodyDesc, Shape * _shape, const core::float4x4 & _matrix, const core::string & _name, core::IObject * _parent);
        Body(PhysicsWorld * _physicsWorld, const PhysicsBodyDesc * _bodyDesc, const core::vector<ShapeInfo> & _shapes, const core::float4x4 & _matrix, const core::string & _name, core::IObject * _parent);
        ~Body();

        void                    Activate            (const core::float4x4 & _world) final override;
        void                    Deactivate          (const core::float4x4 & _world) final override;

        core::float4x4          GetMatrix           () const final override;
        void                    SetMatrix           (core::float4x4 _world)  final override;

        void                    AddImpulse          (const core::float3 & _impulse) final override;

        void                    SetLinearVelocity   (const core::float3 & _velocity) final override;
        core::float3            GetLinearVelocity   () const final override;

        void                    SetAngularVelocity   (const core::float3 & _velocity) final override;
        core::float3            GetAngularVelocity   () const final override;

        const PhysicsBodyDesc * getBodyDesc         () const { return m_bodyDesc;}

    //private:
        void                    createBodyFromJoltShape (PhysicsWorld * _physicsWorld, const PhysicsBodyDesc * _bodyDesc, JPH::Shape * _joltShape, const core::float4x4 & _matrix, const core::string & _name, core::IObject * _parent);
        void                    resetBody               (const core::float4x4 & _world);

    //private:
        const PhysicsWorld *    m_physicsWorld = nullptr;
        const PhysicsBodyDesc * m_bodyDesc = nullptr;
        JPH::BodyID             m_bodyID;
    };  
}