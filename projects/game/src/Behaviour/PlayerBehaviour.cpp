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

    registerProperty(PlayerBehaviour, m_joypadID, "Joypad");
    registerPropertyEnum(PlayerBehaviour, PlayerState, m_state, "State");
    registerPropertyEnum(PlayerBehaviour, Facing, m_facing, "Facing");
    registerProperty(PlayerBehaviour, m_speed, "Speed");
    
    return true;
}

//--------------------------------------------------------------------------------------
void PlayerBehaviour::Update(double _dt)
{
    if (Game::Engine().IsPlaying())
    {
        IInput & input = Game::Input();
        IAnimationComponent * animationComponent = GetGameObject()->GetComponentByType<IAnimationComponent>();
        bool updateRotation = false;

        if (m_joypadID < input.GetJoyCount())
        {
            if (input.IsJoyButtonJustPressed(m_joypadID, JoyButton::A))
            {
                VG_INFO("[Player] Button 'A' pressed on joypad %u", m_joypadID);
                m_state = PlayerState::Hello;
                m_facing = Facing::None;
                updateRotation = true;
            }

            switch (m_state)
            {
                case PlayerState::Idle:
                {
                    if (IAnimationResource * idle = animationComponent->GetAnimation("Idle"))
                    {
                        if (idle && !idle->IsPlaying())
                            idle->PlayLoop();
                    }
                }
                break;

                case PlayerState::Hello:
                {
                    if (IAnimationResource * idle = animationComponent->GetAnimation("Idle"))
                    {
                        if (idle->IsPlaying())
                            idle->Stop();
                    }

                    if (IAnimationResource * hello = animationComponent->GetAnimation("Hello"))
                    {
                        if (!hello->IsPlaying())
                            hello->PlayOnce();
                        else if (hello->IsFinished())
                        {
                            hello->Stop();
                            m_state = PlayerState::Idle;
                            if (IAnimationResource * idle = animationComponent->GetAnimation("Idle"))
                            {
                                if (!idle->IsPlaying())
                                    idle->PlayLoop();
                            }
                        }
                    }
                }

            }

            float3 translation = float3(0, 0, 0);
            const float joyDeadZone = 0.15f;

            const float2 leftJoyDir = input.GetJoyLeftStickDir(m_joypadID);
            if (abs(leftJoyDir).x > joyDeadZone)
                translation.x += leftJoyDir.x * m_speed * _dt * 0.001f;

            if (translation.x > 0.0f && Facing::Left != m_facing)
            {
                m_facing = Facing::Left;
                updateRotation = true;
            }
            else if (translation.x < 0.0f && Facing::Right != m_facing)
            {
                m_facing = Facing::Right;
                updateRotation = true;
            }

            if (abs((float)translation.x) > 0.0f)
            {
                m_state = PlayerState::Walking;
            }

            if (m_state == PlayerState::Walking)
            {
                if (IAnimationResource * walking = animationComponent->GetAnimation("Walking"))
                {
                    if (!walking->IsPlaying())
                        walking->PlayLoop();
                }

                if (abs((float)translation.x) < 0.00001f)
                {
                    if (IAnimationResource * walking = animationComponent->GetAnimation("Walking"))
                    {
                        if (walking->IsPlaying())
                            walking->Stop();
                    }
                    m_state = PlayerState::Idle;
                }
            }

            IGameObject * go = GetGameObject();
            float4x4 worldMatrix = go->GetWorldMatrix();
            {
                if (updateRotation)
                {
                    float4x4 rot;
                    switch (m_facing)
                    {
                        default:
                        VG_ASSERT_ENUM_NOT_IMPLEMENTED(m_facing);
                        break;

                        case None:
                            rot = float4x4::identity();
                            break;

                        case Facing::Left:
                            rot = worldMatrix.rotation_z(PI / 2.0f);
                            break;

                        case Facing::Right:
                            rot = worldMatrix.rotation_z(-PI / 2.0f);
                            break;

                    }

                    worldMatrix[0] = rot[0] * length(worldMatrix[0]);
                    worldMatrix[1] = rot[1] * length(worldMatrix[1]);
                    worldMatrix[2] = rot[2] * length(worldMatrix[2]);
                }

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