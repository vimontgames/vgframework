#include "Precomp.h"
#include "PlayerBehaviour.h"
#include "Game.h"
#include "engine/ICharacterControllerComponent.h"
#include "editor/Editor_Consts.h"
#include "core/GameObject/GameObject.h"
#include "core/IWorld.h"

using namespace vg::core;
using namespace vg::engine;

VG_REGISTER_COMPONENT_CLASS(PlayerBehaviour, "Player", "Game", "Player Behaviour", vg::editor::style::icon::Script, -2); 

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

    registerPropertyGroupBegin(PlayerBehaviour, "Player");
    {
        registerPropertyEnum(PlayerBehaviour, vg::core::InputType, m_controllerType, "Input");
        registerProperty(PlayerBehaviour, m_controllerIndex, "Index");
    }
    registerPropertyGroupEnd(PlayerBehaviour);
    
    return true;
}

//--------------------------------------------------------------------------------------
void PlayerBehaviour::OnPlay() 
{
    super::OnPlay();
    Game::get()->addCharacter(CharacterType::Player, this);
}

//--------------------------------------------------------------------------------------
void PlayerBehaviour::OnStop()
{
    Game::get()->removeCharacter(CharacterType::Player, this);
    super::OnStop();
}

//--------------------------------------------------------------------------------------
void PlayerBehaviour::FixedUpdate(float _dt)
{
    auto * world = GetGameObject()->GetWorld();

    if (world->IsPlaying() && !world->IsPaused())
    {
        IInput & input = Game::Input();
        IAnimationComponent * animationComponent = GetGameObject()->GetComponentByType<IAnimationComponent>();
       
        switch (m_state)
        {
            case CharacterState::Idle:
                PlayAnim(CharacterState::Idle, true);
            break;

            case CharacterState::Walking:
                PlayAnim(CharacterState::Walking, true);
                break;

            case CharacterState::Running:
                PlayAnim(CharacterState::Running, true);
                break;     

            case CharacterState::Jumping:
                PlayAnim(CharacterState::Jumping, false);
                break;
        }

        if (m_controllerType == InputType::Joypad && m_controllerIndex < input.GetJoyCount())
        {
            auto joyID = m_controllerIndex;

            float3 translation = float3(0, 0, 0);
            const float joyDeadZone = 0.15f;

            bool running = input.IsJoyButtonPressed(joyID, JoyButton::X);
            m_currentSpeed = running ? m_runSpeed : m_walkSpeed;

            const float2 leftJoyDir = input.GetJoyLeftStickDir(joyID);

            if (any(abs(leftJoyDir).xy > joyDeadZone))
            {
                translation.xy += leftJoyDir.xy * float2(1, -1) * m_currentSpeed;
                m_currentRotation = radiansToDegrees(atan2((float)leftJoyDir.x, (float)leftJoyDir.y));

                if (!m_isActive)
                    m_isActive = true;
            }

            if (any(abs(translation.xy) > 0.0f))
            {
                if (m_currentSpeed >= (m_walkSpeed + m_runSpeed) * 0.5f)
                    m_state = CharacterState::Running;
                else
                    m_state = CharacterState::Walking;
            }

            if (m_state == CharacterState::Walking || m_state == CharacterState::Running)
            {
                if (abs((float)translation.x) < 0.0001f)
                    m_state = CharacterState::Idle;
            }

            vg::engine::ICharacterControllerComponent * charaController = GetGameObject()->GetComponentByType<vg::engine::ICharacterControllerComponent>();

            bool jump = false;
            if (input.IsJoyButtonJustPressed(joyID, JoyButton::A))
            {
                if (charaController && vg::physics::GroundState::Grounded == charaController->GetGroundState())
                    jump = true;
            }

            if (charaController && vg::physics::GroundState::InTheAir == charaController->GetGroundState())
            {
                m_state = CharacterState::Jumping;

                if (IAnimationResource * anim = animationComponent->GetAnimation(m_anim[CharacterState::Jumping]))
                {
                    if (anim->IsFinished())
                        m_state = CharacterState::Idle;
                }
            }

            if (charaController)
            {
                float3 currentVelocity = charaController->GetVelocity();
                float3 targetVelocity = translation.xyz;
                float3 updatedVelocity;
                updatedVelocity.xy = 0.75f * currentVelocity.xy + 0.25f * targetVelocity.xy;
                updatedVelocity.z = currentVelocity.z;

                if (abs((float)currentVelocity.z) <= 0.0001f && m_state == CharacterState::Jumping)
                    m_state = CharacterState::Idle;

                if (jump)
                    updatedVelocity += float3(0, 0, running? m_runJumpSpeed : m_jumpSpeed);

                charaController->SetVelocity(updatedVelocity);
                charaController->SetRotation(quaternion::rotation_z(degreesToRadians(m_currentRotation)));
            }
        }
        else
        {
            VG_WARNING_ONCE("[Player] %s %u is not a valid controller", asString(m_controllerType).c_str(), m_controllerIndex);
        }
    }
}

//--------------------------------------------------------------------------------------
void PlayerBehaviour::Update(float _dt)
{
    auto * go = GetGameObject();
    auto world = go->getGlobalMatrix();
    if (world[3].z < -32.0f)
    {
        if (auto * charaController = go->GetComponentByType<vg::engine::ICharacterControllerComponent>())
        {
            charaController->SetPosition(m_startPos + float3(0, 0, 16));
        }
    }
}