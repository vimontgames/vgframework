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

    class Body : public IBody
    {
    public:
        VG_CLASS_DECL(Body, IBody);
        VG_CLASS_CTOR_HEADER_IMPL(Body, IBody);

        Body(PhysicsWorld * _physicsWorld, const PhysicsBodyDesc * _bodyDesc, Shape * _shape, const core::float4x4 & _matrix);
        ~Body();

        void Activate(const core::float4x4 & _world) final override;
        void Deactivate(const core::float4x4 & _world) final override;

        core::float4x4 GetMatrix() const final override;

    private:
        void resetBody(const core::float4x4 & _world);

    private:
        const PhysicsWorld *    m_physicsWorld = nullptr;
        const Shape *           m_shape = nullptr;
        const PhysicsBodyDesc *        m_bodyDesc = nullptr;
        JPH::BodyID             m_bodyID;
    };  
}