#include "engine/Precomp.h"
#include "FreeCamBehaviour.h"
#include "core/GameObject/GameObject.h"
#include "core/Math/Math.h"
#include "core/IInput.h"
#include "gfx/IView.h"
#include "engine/Component/Camera/CameraComponent.h"
#include "engine/Engine.h"
#include "renderer/IRenderer.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_COMPONENT_CLASS(FreeCamBehaviour, "FreeCamBehaviour Component");

    //--------------------------------------------------------------------------------------
    bool FreeCamBehaviour::registerProperties(IClassDesc& _desc)
    {
        super::registerProperties(_desc);

        _desc.registerProperty(FreeCamBehaviour, m_moveSpeed, "Translation Speed");
        _desc.setPropertyRange(FreeCamBehaviour, m_moveSpeed, float2(0.0f, 10.0f));

        _desc.registerProperty(FreeCamBehaviour, m_rotSpeed, "Rotation Speed");
        _desc.setPropertyRange(FreeCamBehaviour, m_rotSpeed, float2(0.0f, 10.0f));

        _desc.registerProperty(FreeCamBehaviour, m_pitch, "Pitch");
        _desc.setPropertyRange(FreeCamBehaviour, m_pitch, float2(-PI, +PI));

        _desc.registerProperty(FreeCamBehaviour, m_yaw, "Yaw");
        _desc.setPropertyRange(FreeCamBehaviour, m_yaw, float2(-PI, +PI));

        _desc.registerProperty(FreeCamBehaviour, m_roll, "Roll");
        _desc.setPropertyRange(FreeCamBehaviour, m_roll, float2(-PI, +PI));

        return true;
    }

    //--------------------------------------------------------------------------------------
    FreeCamBehaviour::FreeCamBehaviour(const core::string& _name, core::IObject * _parent) :
        Behaviour(_name, _parent),
        m_pitch(-PI / 2.0f),
        m_yaw(PI),
        m_roll(0.0f),
        m_moveSpeed(1.0f),
        m_rotSpeed(1.0f)
    {
        m_camera = getGameObject()->findComponent<CameraComponent>();
    }

    //--------------------------------------------------------------------------------------
    FreeCamBehaviour::~FreeCamBehaviour()
    {
      
    }

    //--------------------------------------------------------------------------------------
    void FreeCamBehaviour::Update(double _dt)
    {
        bool update = false;

        if (m_camera)
        {
            auto cameraViewID = m_camera->getViewID();
            auto * view = Engine::get()->GetRenderer()->GetView(cameraViewID);

            if (view && view->IsActive())
            {
                if (view->GetViewID() == cameraViewID)
                    update = true;
            }
        }

        auto go = getGameObject();

        const float4x4 & world = go->getWorldMatrix();

        float4 I(world.vec0);
        float4 J(world.vec1);
        float4 K(world.vec2);
        float4 T(world.vec3);

        if (update)
        {
            IInput * input = Kernel::getInput();

            float mouseSpeedX = m_rotSpeed * 0.001f * PI;
            float mouseSpeedY = m_rotSpeed * 0.001f * PI;
            float moveSpeed = m_moveSpeed * 0.001f * (float)_dt;

            if (input->IsMouseButtonPressed(MouseButton::Middle))
            {
                const auto delta = input->GetMouseDelta();

                m_pitch += clamp((float)delta.y * mouseSpeedY, -PI, PI);
                m_yaw -= clamp((float)delta.x * mouseSpeedX, -PI, PI);
            }

            if (m_pitch < -PI)
                m_pitch = 2.0f * PI + m_pitch;
            else if (m_pitch > PI)
                m_pitch = m_pitch - 2.0f * PI;

            if (m_yaw < -PI)
                m_yaw = 2.0f * PI + m_yaw;
            else if (m_yaw > PI)
                m_yaw = m_yaw - 2.0f * PI;

            if (input->IsKeyPressed(Key::A))
                T -= moveSpeed * I;
            else if (input->IsKeyPressed(Key::D))
                T += moveSpeed * I;

            if (input->IsKeyPressed(Key::PAGEDOWN))
                T -= moveSpeed * J;
            else if (input->IsKeyPressed(Key::PAGEUP))
                T += moveSpeed * J;

            if (input->IsKeyPressed(Key::W))
                T -= moveSpeed * K;
            else if (input->IsKeyPressed(Key::S))
                T += moveSpeed * K;
        }

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
    float4x4 FreeCamBehaviour::getRotX(float _alpha)
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
    float4x4 FreeCamBehaviour::getRotY(float _alpha)
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
    float4x4 FreeCamBehaviour::getRotZ(float _alpha)
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