#pragma once

#include "physics/IBody.h"

namespace JPH
{
    class BodyInterface;
}

namespace vg::physics
{
    class BodyDesc;
    class RigidBodyDesc;
    class Shape;

    class Body : public IBody
    {
    public:
        VG_CLASS_DECL(Body, IBody);
        VG_CLASS_CTOR_HEADER_IMPL(Body, IBody);

        Body(const BodyDesc * _bodyDesc, Shape * _shape, const core::float4x4 & _world);
        ~Body();

        void Activate(const core::float4x4 & _world) final override;
        void Deactivate(const core::float4x4 & _world) final override;

        core::float4x4 GetMatrix() const final override;

    private:
        JPH::BodyInterface & getBodyInterface() const;
        void resetBody(const core::float4x4 & _world);

    private:
        const Shape *       m_shape = nullptr;
        const BodyDesc *    m_bodyDesc = nullptr;
        JPH::BodyID         m_bodyID;
    };  

    class RigidBody : public Body
    {
    public:
        VG_CLASS_DECL(RigidBody, Body);
        VG_CLASS_CTOR_HEADER_IMPL(RigidBody, Body);
        
        RigidBody(const RigidBodyDesc * _bodyDesc, Shape * _shape, const core::float4x4 & _world);
        ~RigidBody();
    };
}