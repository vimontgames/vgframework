#include "engine/Precomp.h"
#include "FreeCamComponent.h"
#include "core/GameObject/GameObject.h"
#include "core/Math/Math.h"
#include "core/IInput.h"

using namespace vg::core;

namespace vg::engine
{
    VG_AUTO_REGISTER_CLASS(FreeCamComponent);

    //--------------------------------------------------------------------------------------
    bool FreeCamComponent::registerClass(IFactory& _factory)
    {
        if (core::IClassDesc* desc = _factory.registerClassHelper(FreeCamComponent, "FreeCam Component", IClassDesc::Flags::Component))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool FreeCamComponent::registerProperties(IClassDesc& _desc)
    {
        super::registerProperties(_desc);

        _desc.registerPropertyHelper(FreeCamComponent, m_moveSpeed, "Translation Speed", IProperty::Flags::None);
        _desc.setPropertyRangeHelper(FreeCamComponent, m_moveSpeed, float2(0.0f, 10.0f));

        _desc.registerPropertyHelper(FreeCamComponent, m_rotSpeed, "Rotation Speed", IProperty::Flags::None);
        _desc.setPropertyRangeHelper(FreeCamComponent, m_rotSpeed, float2(0.0f, 10.0f));

        _desc.registerPropertyHelper(FreeCamComponent, m_pitch, "Pitch", IProperty::Flags::None);
        _desc.setPropertyRangeHelper(FreeCamComponent, m_pitch, float2(-pi, +pi));

        _desc.registerPropertyHelper(FreeCamComponent, m_yaw, "Yaw", IProperty::Flags::None);
        _desc.setPropertyRangeHelper(FreeCamComponent, m_yaw, float2(-pi, +pi));

        _desc.registerPropertyHelper(FreeCamComponent, m_roll, "Roll", IProperty::Flags::None);
        _desc.setPropertyRangeHelper(FreeCamComponent, m_roll, float2(-pi, +pi));

        return true;
    }

    //--------------------------------------------------------------------------------------
    FreeCamComponent::FreeCamComponent(const core::string& _name, core::IObject* _parent) :
        CameraComponent(_name, _parent),
        m_roll(0.0f),
        m_pitch(-1.34f),
        m_yaw(2.85f),
        m_moveSpeed(1.0f),
        m_rotSpeed(1.0f)
    {

    }

    //--------------------------------------------------------------------------------------
    FreeCamComponent::~FreeCamComponent()
    {
      
    }

    //--------------------------------------------------------------------------------------
    void FreeCamComponent::Update(double _dt)
    {
        auto go = getGameObject();

        const float4x4 & world = go->getWorldMatrix();

        float4 I(world.vec0);
        float4 J(world.vec1);
        float4 K(world.vec2);
        float4 T(world.vec3);

        IInput * input = Kernel::getInput();

        float mouseSpeedX = m_rotSpeed * 0.001f * pi;
        float mouseSpeedY = m_rotSpeed * 0.001f * pi;
        float moveSpeed = m_moveSpeed * 0.001f * (float)_dt;

        if (input->isMouseButtonPressed(MouseButton::Middle))
        {
            const auto delta = input->getMouseDelta();

            m_pitch += clamp((float)delta.y * mouseSpeedY, -pi, pi);
            m_yaw -= clamp((float)delta.x * mouseSpeedX, -pi, pi);
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

        float4x4 mViewI = float4x4(-I, -J, K, T);

        go->setWorldMatrix(mViewI);

        super::Update(_dt);
    }

    //--------------------------------------------------------------------------------------
    float4x4 FreeCamComponent::getRotX(float _alpha)
    {
        float s = sin(_alpha);
        float c = cos(_alpha);

        return float4x4
        (
            1, 0, 0, 0,
            0, c, s, 0,
            0, -s, c, 0,
            0, 0, 0, 1
        );
    }

    //--------------------------------------------------------------------------------------
    float4x4 FreeCamComponent::getRotY(float _alpha)
    {
        float s = sin(_alpha);
        float c = cos(_alpha);

        return float4x4
        (
            c, 0, -s, 0,
            0, 1, 0, 0,
            s, 0, c, 0,
            0, 0, 0, 1
        );
    }

    //--------------------------------------------------------------------------------------
    float4x4 FreeCamComponent::getRotZ(float _alpha)
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
}