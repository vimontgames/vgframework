#include "Precomp.h"
#include "PlayerBehaviour.h"
#include "physics/IPhysics.h"
#include "physics/IPhysicsOptions.h"
#include "engine/IVehicleComponent.h"

#if !VG_ENABLE_INLINE
#include "PlayerBehaviour.inl"
#endif

#include "core/IBaseScene.h"

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

    registerPropertyEnum(PlayerBehaviour, InputType, m_controllerType, "Input");
    setPropertyDescription(PlayerBehaviour, m_controllerType, "Input type used by player");

    registerProperty(PlayerBehaviour, m_controllerIndex, "Index");
    setPropertyDescription(PlayerBehaviour, m_controllerIndex, "Input index used to differenciate different inputs (e.g., Joypads)");

    registerPropertyEx(PlayerBehaviour, m_customColor, "Custom Color", PropertyFlags::Color);
    setPropertyDescription(PlayerBehaviour, m_customColor, "Custom color used for elements controller by this player");

    registerProperty(PlayerBehaviour, m_UI, "UI");
    setPropertyDescription(PlayerBehaviour, m_UI, "Player UI Prefab instance linked");

    registerPropertyGroupBegin(PlayerBehaviour, "Player debug");
    {
        registerPropertyEx(PlayerBehaviour, m_viewIndex, "View", vg::core::PropertyFlags::Transient | vg::core::PropertyFlags::ReadOnly);
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

    // Hide character
    show(false);

    // Hide UI until the player is active
    if (auto * uiGO = m_UI.get<IGameObject>())
        uiGO->SetInstanceFlags(InstanceFlags::Enabled, false);
}

//--------------------------------------------------------------------------------------
void PlayerBehaviour::OnStop()
{
    m_rightHandItem = nullptr;
    super::OnStop();
    show(true);
}

//--------------------------------------------------------------------------------------
void PlayerBehaviour::FixedUpdate(const Context & _context)
{
    if (_context.m_playing && !_context.m_paused)
    {
        const float time = Game::get()->Engine().GetTime().m_enlapsedTimeSinceStartScaled;

        IInput & input = Game::Input();
        IGameObject * playerGO = GetGameObject();

        if (IGameObject * vehicleGO = (IGameObject*)m_vehicle.getObject())
        {
            // Player is inside vehicle (TODO: check if driver)
            if (IVehicleComponent * vehicle = vehicleGO->GetComponentT<IVehicleComponent>())
            {
                if (m_controllerType == InputType::Joypad && m_controllerIndex < input.GetJoyCount())
                {
                    auto joyID = m_controllerIndex;

                    float backward = input.GetJoyLeftTrigger(joyID);
                    float forward = input.GetJoyRightTrigger(joyID);

                    float forwardSpeed = vehicle->GetForwardVelocity();
                    VG_INFO("[Player] ForwardSpeed = %.2f", forwardSpeed);

                    if (forward > 0.1f)
                    {
                        if (forwardSpeed > 0.0)
                        {
                            vehicle->Accelerate(forward);
                            vehicle->Brake(0.0f);
                        }
                        else
                        {
                            vehicle->Brake(1.0f);
                        }
                    }
                    else if (backward > 0.1f)
                    {
                        if (forwardSpeed < 0)
                        {
                            vehicle->Accelerate(-backward);
                            vehicle->Brake(0.0f);
                        }
                        else
                        {
                            vehicle->Brake(1.0f);
                        }
                    }
                    else
                    {
                        vehicle->Accelerate(0.0f);
                    }

                    float2 dir = input.GetJoyLeftStickDir(joyID);
                    if ((float)(abs(dir.x)) > 0.1f)
                        vehicle->Steer(dir.x);

                    GetGameObject()->SetGlobalMatrix(vehicleGO->GetGlobalMatrix());
                }
            }
        }


        IAnimationComponent * animationComponent = playerGO->GetComponentT<IAnimationComponent>();
       
        switch (m_moveState)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(m_moveState);
                break;

            case MoveState::Die:
                playMoveAnim(MoveState::Die, false);
                if (m_respawnTime <= 0.0f)
                {
                    m_respawnTime = time + 5.0f;
                }
                else if (time >= m_respawnTime)
                {
                    m_respawnTime = 0.0f;
                    OnDeath(_context);
                }

            break;

            case MoveState::Hurt:
                playMoveAnim(MoveState::Hurt, false);
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

        if (auto * animationComponent = _context.m_gameObject->GetComponentT<IAnimationComponent>())
        {
            if (MoveState::Hurt == m_moveState)
            {
                if (IAnimationResource * anim = animationComponent->GetAnimation(m_moveAnim[asInteger(MoveState::Hurt)]))
                {
                    if (!anim->IsPlaying() || anim->IsFinished())
                        m_moveState = MoveState::Idle;
                }
            }
        }

        switch (m_fightState)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(m_fightState);
                break;

            case FightState::None:
            case FightState::Hit:
            case FightState::Shoot:
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

            if (any(abs(leftJoyDir).xy > joyDeadZone) && (m_moveState != MoveState::Hurt && m_moveState != MoveState::Die))
            {
                translation.xy += leftJoyDir.xy * float2(1, -1) * m_speedCurrent;
                m_currentRotation = radiansToDegrees(atan2((float)leftJoyDir.x, (float)leftJoyDir.y));

                if (!m_isActive)
                {
                    m_viewIndex = (u8)Game::get()->getActivePlayers().size();
                    
                    if (auto * uiGO = m_UI.get<IGameObject>())
                    {
                        show(true);

                        uiGO->SetInstanceFlags(InstanceFlags::Enabled, true);
                        if (auto * uiCanvasComponent = uiGO->GetComponentInChildrenT<IUICanvasComponent>())
                            uiCanvasComponent->SetViewIndex(m_viewIndex);
                    }

                    m_isActive = true;

                    // Update physics body with player flag
                    if (auto * physicsBody = playerGO->GetComponentT<ICharacterControllerComponent>())
                    {
                        auto player1Cat = Game::get()->Engine().GetPhysics()->GetOptions()->GetPhysicsCategory("Player 1");
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

            if (MoveState::Die != m_moveState)
            {
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
                const float time = Game::get()->Engine().GetTime().m_enlapsedTimeSinceStartScaled;

                if (input.IsJoyButtonJustPressed(joyID, JoyButton::X))
                {
                    // Enter closed vehicle if any
                    if (!m_vehicle.getObject())
                    {
                        float closestVehicleDist = 3.0f;

                        auto & vehicles = Game::get()->getVehicles();
                        IGameObject * closestVehicle = nullptr;

                        for (uint i = 0; i < vehicles.size(); ++i)
                        {
                            auto * vehicle = vehicles[i];
                            IGameObject * vehicleGO = vehicle->GetGameObject();

                            const float3 delta = vehicleGO->GetGlobalMatrix()[3].xyz - playerPos;
                            const float dist = length(delta);

                            if (dist < closestVehicleDist)
                            {
                                closestVehicleDist = dist;
                                closestVehicle = vehicle->GetGameObject();
                            }
                        }

                        if (closestVehicle)
                        {
                            VG_INFO("[Player] Found vehicle \"%s\" close to player", closestVehicle->GetFullName().c_str());
                            enterVehicle(closestVehicle);
                        }
                    }

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
                                physicsBody->SetTrigger(true);
                        }
                    }
                    else
                    {
                        // HIT
                        if (FightState::None == m_fightState)
                        {
                            if (auto * weaponBehaviour = dynamic_cast<WeaponBehaviour *>(m_rightHandItem))
                            {
                                switch (weaponBehaviour->getWeaponType())
                                {
                                    case WeaponType::Melee:
                                        m_fightState = FightState::Hit;
                                        playFightAnim(FightState::Hit, false);
                                        if (auto * soundComponent = m_rightHandItem->GetGameObject()->GetComponentT<vg::engine::ISoundComponent>())
                                            soundComponent->Play(0);
                                        break;

                                    case WeaponType::Pistol:
                                        m_fightState = FightState::Shoot;
                                        playFightAnim(FightState::Shoot, false);
                                        m_nextShootTime = time + 0.3f; // first shoot cooldown
                                        break;
                                };
                            }
                        }
                    }
                }

                if (FightState::Hit == m_fightState || FightState::Shoot == m_fightState)
                {
                    if (IAnimationResource * anim = animationComponent->GetAnimation(m_fightAnim[asInteger(m_fightState)]))
                    {
                        if (!anim->IsPlaying() || anim->IsFinished())
                        {
                            if (FightState::Shoot != m_fightState || !input.IsJoyButtonPressed(joyID, JoyButton::X))
                            {
                                stopFightAnim(m_fightState);
                                m_fightState = FightState::None;
                                m_nextShootTime = -1.0f;
                            }
                        }
                    }
                }

                if (IAnimationResource * anim = animationComponent->GetAnimation(m_fightAnim[asInteger(m_fightState)]))
                {
                    if (input.IsJoyButtonPressed(joyID, JoyButton::X) && FightState::Shoot == m_fightState && time >= m_nextShootTime)
                    {
                        // Keep shooting
                        //VG_DEBUGPRINT("[PlayerBehaviour] Shoot\n");

                        //VG_WARNING("[Player] Shoot banana");
                        if (nullptr != m_rightHandItem)
                        {
                            WeaponBehaviour * weapon = VG_SAFE_STATIC_CAST(WeaponBehaviour, m_rightHandItem);
                            if (nullptr != weapon)
                            {
                                GameObject * projectileModelGO = weapon->getProjectile().get<GameObject>();
                                if (nullptr != projectileModelGO)
                                {
                                    IGameObject * newProjectileGO = (IGameObject *)projectileModelGO->Instanciate();
                                    newProjectileGO->SetInstanceFlags(InstanceFlags::Enabled, true);
                                    newProjectileGO->SetObjectRuntimeFlags(ObjectRuntimeFlags::Temporary, true);
                                    projectileModelGO->GetScene()->GetRoot()->AddChild(newProjectileGO);
                                    GameObject * weaponGO = weapon->getGameObject();
                                    float4x4 projectileMatrix = weaponGO->getGlobalMatrix();
                                    float3 projectileDir = normalize(projectileMatrix[1].xyz);
                                    projectileMatrix[3].xyz += projectileDir * 0.6f + float3(0, 0, 0.1f); // TODO: expose projectile placement parameters?
                                    newProjectileGO->SetGlobalMatrix(projectileMatrix);
                                    newProjectileGO->OnPlay();

                                    if (auto * physicsBody = newProjectileGO->GetComponentInChildrenT<vg::engine::IPhysicsBodyComponent>())
                                        physicsBody->AddImpulse(float3(projectileDir.x, projectileDir.y, 0.15f) * 3.5f);

                                    if (auto * item = newProjectileGO->GetComponentInChildrenT<ItemBehaviour>())
                                        item->SetOwner(playerGO); // Make current player the owner of the projectile (e.g. for scoring)

                                    if (auto * soundComponent = m_rightHandItem->GetGameObject()->GetComponentT<vg::engine::ISoundComponent>())
                                        soundComponent->Play(0);

                                    VG_SAFE_RELEASE(newProjectileGO);
                                }
                            }

                            // next shoot cooldown
                            m_nextShootTime = time + 0.3f;
                        }
                    }
                }

                if (input.IsJoyButtonJustPressed(joyID, JoyButton::B))
                {
                    // Kick default all ball items
                    for (uint j = 0; j < enumCount<ItemType>(); ++j)
                    {
                        const auto itemType = (ItemType)j;

                        switch (itemType)
                        {
                            default:
                                continue;

                            case ItemType::Default:
                            case ItemType::Ball:
                                break;
                        }

                        auto & balls = Game::get()->getItem(itemType);
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
                                    float kickStrength = lerp(3.0f, 4.5f, runAmount);
                                    physicsBody->AddImpulse(dir * kickStrength);

                                    // Play sound
                                    if (auto * soundComponent = ballGO->GetComponentT<vg::engine::ISoundComponent>())
                                        soundComponent->Play(0);
                                }
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
                            physicsBody->SetTrigger(false);

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
                        updatedVelocity += float3(0, 0, running ? m_runJumpSpeed : m_jumpSpeed);

                    m_velocityNorm = (updatedVelocity.x != 0 || updatedVelocity.y != 0) ? (float)length(updatedVelocity) / _context.m_dt : 0.0f;

                    charaController->SetVelocity(updatedVelocity);
                    charaController->SetRotation(quaternion::rotation_z(degreesToRadians(m_currentRotation)));
                }
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

    if (_context.m_playing && !_context.m_paused)
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

//--------------------------------------------------------------------------------------
bool PlayerBehaviour::enterVehicle(vg::core::IGameObject * _vehicleGameobject)
{
    IVehicleComponent * vehicleComp = _vehicleGameobject->GetComponentT<IVehicleComponent>();
    VG_ASSERT(vehicleComp);
    VehicleSlotType slotType;
    if (vehicleComp->EnterVehicle(this->GetGameObject(), slotType))
    {
        m_vehicle.set(_vehicleGameobject);
        show(false);
        return true;
    }

    return false;
}

//--------------------------------------------------------------------------------------
bool PlayerBehaviour::exitVehicle()
{
    return false;
}