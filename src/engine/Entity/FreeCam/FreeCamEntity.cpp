#include "engine/Precomp.h"
#include "FreeCamEntity.h"
#include "core/Math/Math.h"
#include "core/IInput.h"
#include "core/Object/AutoRegisterClass.h"  

using namespace vg::core;

namespace vg::engine
{
    VG_AUTO_REGISTER_CLASS(FreeCamEntity)

    //--------------------------------------------------------------------------------------
    bool FreeCamEntity::registerClass(IObjectFactory & _factory)
    {
        if (core::IObjectDescriptor * desc = _factory.registerClassHelper(FreeCamEntity, "FreeCamEntity"))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool FreeCamEntity::registerProperties(IObjectDescriptor & _desc)
    {
        super::registerProperties(_desc);

        _desc.registerPropertyHelper(FreeCamEntity, m_pitch, "Pitch", IPropertyDescriptor::Flags::None);
        _desc.setPropertyRangeHelper(FreeCamEntity, m_pitch, float2(-pi, +pi));

        _desc.registerPropertyHelper(FreeCamEntity, m_yaw, "Yaw", IPropertyDescriptor::Flags::None);
        _desc.setPropertyRangeHelper(FreeCamEntity, m_yaw, float2(-pi, +pi));

        _desc.registerPropertyHelper(FreeCamEntity, m_roll, "Roll", IPropertyDescriptor::Flags::None);
        _desc.setPropertyRangeHelper(FreeCamEntity, m_roll, float2(-pi, +pi));

        return true;
    }

    //--------------------------------------------------------------------------------------
    FreeCamEntity::FreeCamEntity(const core::string & _name) : 
        core::Entity(_name),
        m_pitch(-pi*2.0f/4.0f),
        m_yaw(0.0f),
        m_roll(0.0f)
    {
        setMatrix(float4x4
        (
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 64.0f, 16.0f, 1.0f
        ));
    }

    //--------------------------------------------------------------------------------------
    FreeCamEntity::~FreeCamEntity()
    {

    }

    //--------------------------------------------------------------------------------------
    float4x4 getRotX(float _alpha)
    {
        float s = sin(_alpha);
        float c = cos(_alpha);

        return float4x4
        (
            1, 0, 0, 0,
            0, c, s, 0,
            0,-s, c, 0,
            0, 0, 0, 1
        );
    }

    //--------------------------------------------------------------------------------------
    float4x4 getRotY(float _alpha)
    {
        float s = sin(_alpha);
        float c = cos(_alpha);

        return float4x4
        (
            c, 0,-s, 0,
            0, 1, 0, 0,
            s, 0, c, 0,
            0, 0, 0, 1
        );
    }

    //--------------------------------------------------------------------------------------
    float4x4 getRotZ(float _alpha)
    {
        float s = sin(_alpha);
        float c = cos(_alpha);

        return float4x4
        (
            c, s, 0, 0,
           -s, c, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        );
    }


    //--------------------------------------------------------------------------------------
    void FreeCamEntity::update(double _dt)
    {
        const float4x4 & world = getMatrix();
        float4 I(world.vec0);
        float4 J(world.vec1);
        float4 K(world.vec2);
        float4 T(world.vec3);

        IInput * input = Kernel::getInput();

        float mouseSpeedX = 0.001f * pi;
        float mouseSpeedY = 0.001f * pi;
        float moveSpeed = 0.01f * (float)_dt;

        if (input->isMouseButtonPressed(MouseButton::Middle))
        {
            const auto delta = input->getMouseDelta();

            m_pitch += clamp((float)delta.y * mouseSpeedY, -pi, pi);
            m_yaw += clamp((float)delta.x  * mouseSpeedX, -pi, pi);
        }

        if (m_pitch < -pi)
            m_pitch = 2.0f * pi + m_pitch;
        else if (m_pitch > pi)
            m_pitch = m_pitch - 2.0f * pi;

        if (m_yaw < -pi)
            m_yaw = 2.0f * pi + m_yaw;
        else if (m_yaw > pi)
            m_yaw = m_yaw - 2.0f * pi;

        if (input->isKeyPressed(Key::A))
            T -= moveSpeed * I;
        else if (input->isKeyPressed(Key::D))
            T += moveSpeed * I;

        if (input->isKeyPressed(Key::PAGEDOWN))
            T -= moveSpeed * J;
        else if (input->isKeyPressed(Key::PAGEUP))
            T += moveSpeed * J;

        if (input->isKeyPressed(Key::W))
            T -= moveSpeed * K;
        else if (input->isKeyPressed(Key::S))
            T += moveSpeed * K;

        const float4x4 rotX = getRotX(m_pitch);
        const float4x4 rotZ = getRotZ(m_yaw);
        
        float4x4 mRotXZ = mul(rotX, rotZ);

        I = normalize(float4(mRotXZ.vec0));
        J = normalize(float4(mRotXZ.vec1));
        K = normalize(float4(mRotXZ.vec2));

        float4x4 mViewI = float4x4( I, -J, K, T );
        
        setMatrix(mViewI);

        super::update(_dt);
    }
}