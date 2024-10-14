#include "Precomp.h"
#include "PlayerBehaviour.h"

#if !VG_ENABLE_INLINE
#include "PlayerBehaviour.inl"
#endif

using namespace vg::core;
using namespace vg::engine;

VG_REGISTER_COMPONENT_CLASS(PlayerBehaviour, "Player", "Game", "Player Behaviour", vg::editor::style::icon::Script, -2); 

//--------------------------------------------------------------------------------------
PlayerBehaviour::PlayerBehaviour(const string & _name, IObject * _parent) :
    super(_name, _parent, CharacterType::Player)
{
    EnableUpdateFlags(UpdateFlags::FixedUpdate | UpdateFlags::Update);

    m_life = 3;
    m_hp = 100.0f;
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
        registerPropertyEnum(PlayerBehaviour, InputType, m_controllerType, "Input");
        setPropertyDescription(PlayerBehaviour, m_controllerType, "Input type used by player");

        registerProperty(PlayerBehaviour, m_controllerIndex, "Index");
        setPropertyDescription(PlayerBehaviour, m_controllerIndex, "Input index used to differenciate different inputs (e.g. Joypads)");

        registerPropertyEx(PlayerBehaviour, m_customColor, "Custom Color", PropertyFlags::Color);
        setPropertyDescription(PlayerBehaviour, m_customColor, "Custom color used for elements controller by this player");

        registerProperty(PlayerBehaviour, m_UI, "UI");
        setPropertyDescription(PlayerBehaviour, m_UI, "Player UI Prefab instance linked");

        registerPropertyEx(PlayerBehaviour, m_viewIndex, "View", vg::core::PropertyFlags::NotSaved | vg::core::PropertyFlags::ReadOnly);
        setPropertyDescription(PlayerBehaviour, m_viewIndex, "Index of the View used by this player");
    }
    registerPropertyGroupEnd(PlayerBehaviour);
    
    return true;
}

//--------------------------------------------------------------------------------------
void PlayerBehaviour::OnPlay()
{
    super::OnPlay();

    // Compute initial rotation around Z axis
    const auto global = GetGameObject()->GetGlobalMatrix();
    m_currentRotation = -atan2(global[1].x, global[0].x) * 180.0f / PI;

    // Hide UI until the player is active
    if (auto * uiGO = m_UI.get<IGameObject>())
        uiGO->SetInstanceFlags(InstanceFlags::Enabled, false);
}

//--------------------------------------------------------------------------------------
void PlayerBehaviour::OnStop()
{
    m_isActive = false;
    m_rightHandItem = nullptr;
    super::OnStop();
}

//--------------------------------------------------------------------------------------
void PlayerBehaviour::FixedUpdate(const Context & _context)
{
    if (_context.m_world->IsPlaying() && !_context.m_world->IsPaused()) // TODO: Use context intead?
    {
        IInput & input = Game::Input();
        IGameObject * playerGO = GetGameObject();
        IAnimationComponent * animationComponent = playerGO->GetComponentT<IAnimationComponent>();
       
        switch (m_moveState)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(m_moveState);
                break;

            case MoveState::Idle:
                playMoveAnim(MoveState::Idle, true);
            break;

            case MoveState::Walk:
                playMoveAnim(MoveState::Walk, true);
                break;

            case MoveState::Run:
                playMoveAnim(MoveState::Run, true);
                break;     

            case MoveState::Jump:
                playMoveAnim(MoveState::Jump, true);
                break;
        }

        switch (m_fightState)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(m_fightState);
                break;

            case FightState::None:
            case FightState::Hit:
            case FightState::Kick:
                break;
        }

        if (m_controllerType == InputType::Joypad && m_controllerIndex < input.GetJoyCount())
        {
            auto joyID = m_controllerIndex;

            float3 translation = float3(0, 0, 0);
            const float joyDeadZone = 0.15f;

            float runAmount = input.GetJoyRightTrigger(joyID);
            if (runAmount > joyDeadZone)
                runAmount = lerp(0.0f, 1.0f, saturate(runAmount - joyDeadZone) / (1.0f - joyDeadZone));
            else
                runAmount = 0.0f;

            bool running = runAmount > 0.0f;

            m_speedCurrent = lerp(m_walkSpeed, m_runSpeed, runAmount);

            const float2 leftJoyDir = input.GetJoyLeftStickDir(joyID);

            if (any(abs(leftJoyDir).xy > joyDeadZone))
            {
                translation.xy += leftJoyDir.xy * float2(1, -1) * m_speedCurrent;
                m_currentRotation = radiansToDegrees(atan2((float)leftJoyDir.x, (float)leftJoyDir.y));

                if (!m_isActive)
                {
                    m_viewIndex = (u8)Game::get()->getActivePlayers().size();
                    
                    if (auto * uiGO = m_UI.get<IGameObject>())
                    {
                        uiGO->SetInstanceFlags(InstanceFlags::Enabled, true);
                        if (auto * uiCanvasComponent = uiGO->GetComponentInChildrenT<IUICanvasComponent>())
                            uiCanvasComponent->SetViewIndex(m_viewIndex);
                    }

                    m_isActive = true;

                    // Update physics body with player flag
                    if (auto * physicsBody = playerGO->GetComponentT<ICharacterControllerComponent>())
                    {
                        auto player1Cat = Game::get()->Engine().GetOptions()->GetPhysicsCategory("Player 1");
                        physicsBody->SetCategory((vg::physics::Category)((uint)player1Cat + m_viewIndex));
                    }
                }
            }

            if (any(abs(translation.xy) > 0.0f))
            {
                if (m_speedCurrent >= (m_walkSpeed + m_runSpeed) * 0.5f)
                    m_moveState = MoveState::Run;
                else
                    m_moveState = MoveState::Walk;
            }

            if (m_moveState == MoveState::Walk || m_moveState == MoveState::Run)
            {
                if (abs((float)translation.x) < 0.0001f)
                    m_moveState = MoveState::Idle;
            }

            vg::engine::ICharacterControllerComponent * charaController = playerGO->GetComponentT<vg::engine::ICharacterControllerComponent>();

            bool jump = false;
            if (input.IsJoyButtonJustPressed(joyID, JoyButton::A))
            {
                if (charaController && vg::physics::GroundState::Grounded == charaController->GetGroundState())
                    jump = true;
            }

            if (charaController && vg::physics::GroundState::InAir == charaController->GetGroundState())
            {
                m_moveState = MoveState::Jump;

                if (IAnimationResource * anim = animationComponent->GetAnimation(m_moveAnim[asInteger(MoveState::Jump)]))
                {
                    if (anim->IsFinished())
                        m_moveState = MoveState::Idle;
                }                    
            } 

            const float pickupDist = 1.0f;
            const float3 playerPos = playerGO->GetGlobalMatrix()[3].xyz;

            if (input.IsJoyButtonJustPressed(joyID, JoyButton::X))
            {
                // Pick closest weapon
                if (nullptr == m_rightHandItem)
                {
                    auto & weapons = Game::get()->getWeapons();
                    float closestDist = pickupDist + 1;
                    IGameObject * closestWeaponGO = nullptr;
                    WeaponBehaviour * closestWeaponBehaviour = nullptr;
                    for (uint i = 0; i < weapons.size(); ++i)
                    {
                        auto * weaponBehaviour = weapons[i];
                        IGameObject * weaponGO = weaponBehaviour->GetGameObject();
                        float3 delta = weaponGO->GetGlobalMatrix()[3].xyz - playerPos;
                        float dist = length(delta);
                        if (dist < closestDist && dist < pickupDist && !weaponBehaviour->CanPick())
                        {
                            closestDist = dist;
                            closestWeaponBehaviour = weaponBehaviour;
                            closestWeaponGO = closestWeaponBehaviour->GetGameObject();
                        }
                    }

                    if (closestWeaponBehaviour)
                    {
                        closestWeaponBehaviour->SetOwner(playerGO);
                        m_rightHandItem = closestWeaponBehaviour;

                        if (auto * physicsBody = closestWeaponGO->GetComponentT<vg::engine::IPhysicsBodyComponent>())
                        {
                            physicsBody->SetTrigger(true);

                            //// Weapon still collides with other physic objects but is moved by code
                            //physicsBody->SetMotionType(vg::physics::MotionType::Kinematic);
                            //
                            //// Weapon should collide with everything but the players
                            //auto player1Cat = Game::get()->Engine().GetOptions()->GetPhysicsCategory("Player 1");
                            //auto mask = (vg::physics::CategoryFlag)0x0;
                            //for (uint i = 0; i < 4; ++i)
                            //    mask |= (vg::physics::CategoryFlag)(1ULL << ((u64)player1Cat + i));
                            //physicsBody->SetCollisionMask(~mask);
                            //
                            //physicsBody->EnableCollisionMask(true);
                        }
                    }
                }
                else
                {
                    // HIT
                    if (FightState::None == m_fightState)
                    {
                        m_fightState = FightState::Hit;
                        playFightAnim(FightState::Hit, false);

                        // Play sound
                        if (auto * soundComponent = m_rightHandItem->GetGameObject()->GetComponentT<vg::engine::ISoundComponent>())
                            soundComponent->Play(0);
                    }
                }
            }

            if (FightState::Hit == m_fightState)
            {
                if (IAnimationResource * anim = animationComponent->GetAnimation(m_fightAnim[asInteger(FightState::Hit)]))
                {
                    if (anim->IsFinished())
                    {
                        stopFightAnim(FightState::Hit);
                        m_fightState = FightState::None;
                    }
                }
            }

            if (input.IsJoyButtonJustPressed(joyID, JoyButton::B))
            {
                // Kick close balls
                auto & balls = Game::get()->getBalls();
                for (uint i = 0; i < balls.size(); ++i)
                {
                    auto * ballBehaviour = balls[i];
                    auto * ballGO = ballBehaviour->GetGameObject();

                    float3 delta = ballGO->GetGlobalMatrix()[3].xyz - playerPos;
                    float dist = length(delta);
                    if (dist < pickupDist)
                    {
                        float3 dir = normalize(delta);
                        ballBehaviour->SetOwner(ballGO);
                        if (auto * physicsBody = ballGO->GetComponentT<vg::engine::IPhysicsBodyComponent>())
                        {
                            //VG_INFO("[Player] Kick ball %u (dist = %.2f, dir = %.2f, %.2f, %.2f)", i, dist, dir.x, dir.y, dir.z);
                            float kickStrength = lerp(250.0f, 400.0f, runAmount);
                            physicsBody->AddImpulse(dir * kickStrength);

                            // Play sound
                            if (auto * soundComponent = ballGO->GetComponentT<vg::engine::ISoundComponent>())
                                soundComponent->Play(0);
                        }
                    }
                }
            }

            if (input.IsJoyButtonJustPressed(joyID, JoyButton::Y))
            {
                if (nullptr != m_rightHandItem)
                {
                    // drop
                    m_rightHandItem->SetOwner(nullptr);

                    if (auto * physicsBody = m_rightHandItem->GetGameObject()->GetComponentT<vg::engine::IPhysicsBodyComponent>())
                    {
                        physicsBody->SetTrigger(false);

                        //physicsBody->SetMotionType(vg::physics::MotionType::Dynamic);
                        //physicsBody->SetMatrix(m_rightHandItem->GetGameObject()->GetGlobalMatrix());
                        //
                        //// Weapon should collide with everything again
                        //physicsBody->EnableCollisionMask(false);
                        //physicsBody->SetCollisionMask((vg::physics::CategoryFlag)-1);
                    }

                    m_rightHandItem = nullptr;
                }
            }

            if (charaController)
            {
                float3 currentVelocity = charaController->GetVelocity();
                float3 targetVelocity = translation.xyz;
                float3 updatedVelocity;
                updatedVelocity.xy = smoothdamp(currentVelocity.xy, targetVelocity.xy, m_velocitySmoothdamp, 0.02f, _context.m_dt);
                updatedVelocity.z = currentVelocity.z;

                if (abs((float)currentVelocity.z) <= 0.0001f && m_moveState == MoveState::Jump)
                    m_moveState = MoveState::Idle;

                if (jump)
                    updatedVelocity += float3(0, 0, running? m_runJumpSpeed : m_jumpSpeed);

                m_velocityNorm = (updatedVelocity.x != 0 || updatedVelocity.y != 0) ? (float)length(updatedVelocity) / _context.m_dt : 0.0f;

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
void PlayerBehaviour::Update(const Context & _context)
{
    super::Update(_context);

    if (_context.m_world->IsPlaying() && !_context.m_world->IsPaused())   // TODO: pass world->IsPlaying and world->IsPaused as 'Update' parameters, or World directly? Use Context?
    {
        // Update UI
        {
            if (IGameObject * uiGO = m_UI.get<IGameObject>())
            {
                if (IGameObject * lifeGO = uiGO->GetChildGameObject("Life"))
                {
                    if (IUITextComponent * lifeComp = lifeGO->GetComponentT<IUITextComponent>())
                        lifeComp->SetText(fmt::sprintf("Life   %u", m_life));
                }

                if (IGameObject * hpGO = uiGO->GetChildGameObject("Health"))
                {
                    if (IUITextComponent * textComp = hpGO->GetComponentT<IUITextComponent>())
                        textComp->SetText(fmt::sprintf("Health %.0f%%", m_hp));
                }

                if (IGameObject * scoreGO = uiGO->GetChildGameObject("Score"))
                {
                    if (IUITextComponent * scoreComp = scoreGO->GetComponentT<IUITextComponent>())
                        scoreComp->SetText(fmt::sprintf("Score  %i", m_score));
                }
            }
        }
    }
}