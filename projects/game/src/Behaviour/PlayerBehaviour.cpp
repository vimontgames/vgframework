#include "Precomp.h"
#include "PlayerBehaviour.h"
#include "Game.h"
#include "engine/ICharacterControllerComponent.h"

using namespace vg::core;
using namespace vg::engine;

VG_REGISTER_COMPONENT_CLASS(PlayerBehaviour, "Player", "Game", "Player Behaviour");

//--------------------------------------------------------------------------------------
PlayerBehaviour::PlayerBehaviour(const string & _name, IObject * _parent) :
    super(_name, _parent)
{
    SetUpdateFlags(UpdateFlags::FixedUpdate | UpdateFlags::Update);
}

//--------------------------------------------------------------------------------------
PlayerBehaviour::~PlayerBehaviour()
{

}

//--------------------------------------------------------------------------------------
bool PlayerBehaviour::registerProperties(IClassDesc & _desc)
{
    super::registerProperties(_desc);

    registerProperty(PlayerBehaviour, m_joypadID, "Joypad");
    registerProperty(PlayerBehaviour, m_walkSpeed, "Walk Speed");
    registerProperty(PlayerBehaviour, m_runSpeed, "Run Speed");
    registerProperty(PlayerBehaviour, m_jumpSpeed, "Jump Speed");
    registerProperty(PlayerBehaviour, m_runJumpSpeed, "Run Jump Speed");

    registerPropertyGroupBegin(CharacterControllerComponent, "Debug");
    registerPropertyEnumEx(PlayerBehaviour, PlayerState, m_state, "State", vg::core::IProperty::Flags::NotSaved);
    registerPropertyEx(PlayerBehaviour, m_currentSpeed, "Speed", vg::core::IProperty::Flags::NotSaved);
    registerPropertyEx(PlayerBehaviour, m_currentRotation, "Rot", vg::core::IProperty::Flags::NotSaved);
    registerPropertyGroupEnd(CharacterControllerComponent);
    
    return true;
}

//--------------------------------------------------------------------------------------
void PlayerBehaviour::OnPlay() 
{
    super::OnPlay();

    for (uint i = 0; i < vg::core::countof(m_anim); ++i)
        m_anim[i] = -1;

    IAnimationComponent * animationComponent = GetGameObject()->GetComponentByType<IAnimationComponent>();

    if (nullptr != animationComponent)
    {
        m_anim[PlayerState::Idle] = animationComponent->GetAnimationIndex("Idle");
        m_anim[PlayerState::Walking] = animationComponent->GetAnimationIndex("Walking");
        m_anim[PlayerState::Running] = animationComponent->GetAnimationIndex("Running");
        m_anim[PlayerState::Jumping] = animationComponent->GetAnimationIndex("Jump");
    } 

    Game::get()->addPlayer(this);
}

//--------------------------------------------------------------------------------------
void PlayerBehaviour::OnStop()
{
    Game::get()->removePlayer(this);

    super::OnStop();
}

//--------------------------------------------------------------------------------------
void PlayerBehaviour::PlayAnim(PlayerState _state, bool _loop)
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
        {
            if (_loop)
                anim->PlayLoop();
            else
                anim->PlayOnce();
        }
    }
}

//--------------------------------------------------------------------------------------
void PlayerBehaviour::FixedUpdate(float _dt)
{
    if (Game::Engine().IsPlaying() && !Game::Engine().IsPaused())
    {
        IInput & input = Game::Input();
        IAnimationComponent * animationComponent = GetGameObject()->GetComponentByType<IAnimationComponent>();

        if (m_joypadID < input.GetJoyCount())
        {
            switch (m_state)
            {
                case PlayerState::Idle:
                    PlayAnim(PlayerState::Idle, true);
                break;

                case PlayerState::Walking:
                    PlayAnim(PlayerState::Walking, true);
                    break;

                case PlayerState::Running:
                    PlayAnim(PlayerState::Running, true);
                    break;     

                case PlayerState::Jumping:
                    PlayAnim(PlayerState::Jumping, false);
                    break;
            }

            float3 translation = float3(0, 0, 0);
            const float joyDeadZone = 0.15f;

            bool running = input.IsJoyButtonPressed(m_joypadID, JoyButton::X);
            m_currentSpeed = running ? m_runSpeed : m_walkSpeed;

            const float2 leftJoyDir = input.GetJoyLeftStickDir(m_joypadID);

            if (any(abs(leftJoyDir).xy > joyDeadZone))
            {
                translation.xy += leftJoyDir.xy * float2(1, -1) * m_currentSpeed;
                m_currentRotation = radiansToDegrees(atan2((float)leftJoyDir.x, (float)leftJoyDir.y));
            }

            if (any(abs(translation.xy) > 0.0f))
            {
                if (m_currentSpeed >= (m_walkSpeed + m_runSpeed) * 0.5f)
                    m_state = PlayerState::Running;
                else
                    m_state = PlayerState::Walking;
            }

            if (m_state == PlayerState::Walking || m_state == PlayerState::Running)
            {
                if (abs((float)translation.x) < 0.0001f)
                    m_state = PlayerState::Idle;
            }

            vg::engine::ICharacterControllerComponent * charaController = GetGameObject()->GetComponentByType<vg::engine::ICharacterControllerComponent>();

            bool jump = false;
            if (input.IsJoyButtonJustPressed(m_joypadID, JoyButton::A))
            {
                if (charaController && vg::physics::GroundState::Grounded == charaController->GetGroundState())
                    jump = true;
            }

            if (charaController && vg::physics::GroundState::InTheAir == charaController->GetGroundState())
            {
                m_state = PlayerState::Jumping;

                if (IAnimationResource * anim = animationComponent->GetAnimation(m_anim[PlayerState::Jumping]))
                {
                    if (anim->IsFinished())
                        m_state = PlayerState::Idle;
                }
            }

            if (charaController)
            {
                float3 currentVelocity = charaController->GetVelocity();
                float3 targetVelocity = translation.xyz;
                float3 updatedVelocity;
                updatedVelocity.xy = 0.75f * currentVelocity.xy + 0.25f * targetVelocity.xy;
                updatedVelocity.z = currentVelocity.z;

                if (abs((float)currentVelocity.z) <= 0.0001f && m_state == PlayerState::Jumping)
                    m_state = PlayerState::Idle;

                if (jump)
                    updatedVelocity += float3(0, 0, running? m_runJumpSpeed : m_jumpSpeed);

                charaController->SetVelocity(updatedVelocity);
                charaController->SetRotation(quaternion::rotation_z(degreesToRadians(m_currentRotation)));
            }
        }
        else
        {
            VG_WARNING_ONCE("[Player] Joypad %u is not a valid Joypad (%u detected)", m_joypadID, input.GetJoyCount());
        }
    }
}

//--------------------------------------------------------------------------------------
void PlayerBehaviour::Update(float _dt)
{

}