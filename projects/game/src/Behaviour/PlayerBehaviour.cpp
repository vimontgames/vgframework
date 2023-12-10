#include "Precomp.h"
#include "PlayerBehaviour.h"
#include "Game.h"

using namespace vg::core;
using namespace vg::engine;

VG_REGISTER_COMPONENT_CLASS(PlayerBehaviour, "Player", "Game", "Player Behaviour");

//--------------------------------------------------------------------------------------
PlayerBehaviour::PlayerBehaviour(const string & _name, IObject * _parent) :
    Component(_name, _parent)
{

}

//--------------------------------------------------------------------------------------
PlayerBehaviour::~PlayerBehaviour()
{

}

//--------------------------------------------------------------------------------------
bool PlayerBehaviour::registerProperties(IClassDesc & _desc)
{
    super::registerProperties(_desc);

    registerPropertySeparator(PlayerBehaviour, "Settings");
    registerProperty(PlayerBehaviour, m_joypadID, "Joypad");
    registerPropertyEnum(PlayerBehaviour, PlayerState, m_state, "State");
    registerProperty(PlayerBehaviour, m_walkSpeed, "Walk Speed");
    registerProperty(PlayerBehaviour, m_runSpeed, "Run Speed");

    registerPropertySeparator(PlayerBehaviour, "Debug");
    registerPropertyEx(PlayerBehaviour, m_dbgSpeed, "Speed", vg::core::IProperty::Flags::NotSaved);
    registerPropertyEx(PlayerBehaviour, m_dbgRot, "Rot", vg::core::IProperty::Flags::NotSaved);
    
    return true;
}

//--------------------------------------------------------------------------------------
void PlayerBehaviour::OnPlay() 
{
    for (uint i = 0; i < vg::core::countof(m_anim); ++i)
        m_anim[i] = -1;

    IAnimationComponent * animationComponent = GetGameObject()->GetComponentByType<IAnimationComponent>();

    if (nullptr != animationComponent)
    {
        m_anim[PlayerState::Idle] = animationComponent->GetAnimationIndex("Idle");
        m_anim[PlayerState::Walking] = animationComponent->GetAnimationIndex("Walking");
        m_anim[PlayerState::Running] = animationComponent->GetAnimationIndex("Running");
    }
}

//--------------------------------------------------------------------------------------
void PlayerBehaviour::PlayAnim(PlayerState _state)
{
    IAnimationComponent * animationComponent = GetGameObject()->GetComponentByType<IAnimationComponent>();

    for (uint i = 0; i < vg::core::countof(m_anim); ++i)
    {
        if (_state != i)
        {
            if (IAnimationResource * anim = animationComponent->GetAnimation(m_anim[i]))
            {
                if (anim->IsPlaying())
                    anim->Stop();
            }
        }
    }

    if (IAnimationResource * anim = animationComponent->GetAnimation(m_anim[_state]))
    {
        if (anim && !anim->IsPlaying())
            anim->PlayLoop();
    }
}

//--------------------------------------------------------------------------------------
void PlayerBehaviour::Update(double _dt)
{
    if (Game::Engine().IsPlaying())
    {
        IInput & input = Game::Input();
        IAnimationComponent * animationComponent = GetGameObject()->GetComponentByType<IAnimationComponent>();

        if (m_joypadID < input.GetJoyCount())
        {
            switch (m_state)
            {
                case PlayerState::Idle:
                    PlayAnim(PlayerState::Idle);
                break;

                case PlayerState::Walking:
                    PlayAnim(PlayerState::Walking);
                    break;

                case PlayerState::Running:
                    PlayAnim(PlayerState::Running);
                    break;           
            }

            float3 translation = float3(0, 0, 0);
            const float joyDeadZone = 0.15f;

            bool running = input.IsJoyButtonPressed(m_joypadID, JoyButton::X);
            m_dbgSpeed = running ? m_runSpeed : m_walkSpeed;

            const float2 leftJoyDir = input.GetJoyLeftStickDir(m_joypadID);

            if (any(abs(leftJoyDir).xy > joyDeadZone))
            {
                translation.xy += leftJoyDir.xy * float2(1, -1) * m_dbgSpeed * _dt;
                m_dbgRot = atan2((float)leftJoyDir.x, (float)leftJoyDir.y) * 180.0f / PI;
            }

            if (any(abs(translation.xy) > 0.0f))
            {
                if (m_dbgSpeed >= (m_walkSpeed + m_runSpeed) * 0.5f)
                    m_state = PlayerState::Running;
                else
                    m_state = PlayerState::Walking;
            }

            if (m_state == PlayerState::Walking || m_state == PlayerState::Running)
            {
                if (abs((float)translation.x) < 0.00001f)
                    m_state = PlayerState::Idle;
            }

            IGameObject * go = GetGameObject();
            float4x4 worldMatrix = go->GetWorldMatrix();
            {
                float4x4 mRot = float4x4::rotation_z(m_dbgRot * PI / 180.0f);

                worldMatrix[0] = mRot[0];
                worldMatrix[1] = mRot[1];
                worldMatrix[2] = mRot[2];

                worldMatrix[3].xyz += translation;
            }
            go->setWorldMatrix(worldMatrix);
        }
        else
        {
            VG_WARNING("[Player] %u is not a valid Joypad (%u joypads detected)", m_joypadID, input.GetJoyCount());
        }
    }
}