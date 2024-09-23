#include "engine/Precomp.h"
#include "FreeCamBehaviour.h"
#include "core/GameObject/GameObject.h"
#include "core/Math/Math.h"
#include "core/IInput.h"
#include "gfx/IView.h"
#include "engine/Component/Camera/CameraComponent.h"
#include "engine/Engine.h"
#include "renderer/IRenderer.h"
#include "editor/Editor_Consts.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_COMPONENT_CLASS(FreeCamBehaviour, "Free Camera", "Renderer", "Keyboard/Mouse First-Person Camera behaviour", editor::style::icon::Script, 0);

    //--------------------------------------------------------------------------------------
    bool FreeCamBehaviour::registerProperties(IClassDesc& _desc)
    {
        super::registerProperties(_desc);

        registerProperty(FreeCamBehaviour, m_moveSpeed, "Translation Speed");
        setPropertyRange(FreeCamBehaviour, m_moveSpeed, float2(0.0f, 10.0f));

        registerProperty(FreeCamBehaviour, m_rotSpeed, "Rotation Speed");
        setPropertyRange(FreeCamBehaviour, m_rotSpeed, float2(0.0f, 10.0f));

        registerProperty(FreeCamBehaviour, m_pitch, "Pitch");
        setPropertyRange(FreeCamBehaviour, m_pitch, float2(-PI, +PI));

        registerProperty(FreeCamBehaviour, m_yaw, "Yaw");
        setPropertyRange(FreeCamBehaviour, m_yaw, float2(-PI, +PI));

        registerProperty(FreeCamBehaviour, m_roll, "Roll");
        setPropertyRange(FreeCamBehaviour, m_roll, float2(-PI, +PI));

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
        m_camera = GetGameObject()->GetComponentT<CameraComponent>();
    }

    //--------------------------------------------------------------------------------------
    FreeCamBehaviour::~FreeCamBehaviour()
    {
      
    }

    //--------------------------------------------------------------------------------------
    void FreeCamBehaviour::Update(const Context & _context)
    {
        bool update = false;

        if (m_camera)
        {
            auto cameraViewID = m_camera->getViewID();
            auto * view = Engine::get()->GetRenderer()->GetView(cameraViewID);

            if (view && view->IsFocused())
            {
                if (view->GetViewID() == cameraViewID)
                    update = true;
            }
        }

        const float4x4 & world = _context.m_gameObject->getGlobalMatrix();

        float4 I = world[0];
        float4 J = world[1];
        float4 K = world[2];
        float4 T = world[3];

        IInput * input = Kernel::getInput();
        if (update && input->IsKeyPressed(Key::LeftShift))
        {
            float mouseSpeedX = m_rotSpeed * 0.001f * PI;
            float mouseSpeedY = m_rotSpeed * 0.001f * PI;
            float moveSpeed = m_moveSpeed * _context.m_dt * 8.0f;

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

            if (!input->IsKeyPressed(Key::LeftControl))
            {
                if (input->IsKeyPressed(Key::A))
                    T -= moveSpeed * I;
                else if (input->IsKeyPressed(Key::D))
                    T += moveSpeed * I;

                if (input->IsKeyPressed(Key::PageDown))
                    T -= moveSpeed * J;
                else if (input->IsKeyPressed(Key::PageUp))
                    T += moveSpeed * J;

                if (input->IsKeyPressed(Key::W))
                    T -= moveSpeed * K;
                else if (input->IsKeyPressed(Key::S))
                    T += moveSpeed * K;
            }
        }

        const float4x4 rotX = getRotX(m_pitch);
        const float4x4 rotZ = getRotZ(m_yaw);

        float4x4 mRotXZ = mul(rotX, rotZ);

        I = normalize(float4(mRotXZ[0]));
        J = normalize(float4(mRotXZ[1]));
        K = normalize(float4(mRotXZ[2]));

        float4x4 mViewI = float4x4(-I, -J, K, T);

        _context.m_gameObject->setGlobalMatrix(mViewI);
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
            0,-s, c, 0,
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
            c, 0,-s, 0,
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