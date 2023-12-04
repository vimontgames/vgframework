#pragma once

#include "physics/IBody.h"
#include "Jolt/Physics/Body/BodyID.h"

namespace JPH
{
    class BodyInterface;
}

namespace vg::physics
{
    class Body final : public IBody
    {
    public:
        VG_CLASS_DECL(Body, IBody);

        Body(const core::string & _name, core::IObject * _parent);
        Body(const JPH::BodyID _joltBodyID, const core::float4x4 & _transform);
        ~Body();

        void Activate(const core::float4x4 & _world) final override;
        void Deactivate(const core::float4x4 & _world) final override;

        core::float4x4 GetMatrix() const final override;

    private:
        JPH::BodyInterface & getBodyInterface() const;
        void resetBody(const core::float4x4 & _world);

    private:
        core::float4x4 m_transform = core::float4x4::identity();
        JPH::BodyID m_joltBodyID;
    };    
}