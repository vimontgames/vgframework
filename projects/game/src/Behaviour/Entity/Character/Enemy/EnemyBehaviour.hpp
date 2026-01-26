#include "Precomp.h"
#include "EnemyBehaviour.h"

using namespace vg::core;
using namespace vg::engine;

VG_REGISTER_COMPONENT_CLASS(EnemyBehaviour, "Enemy", "Game", "Enemy Behaviour", vg::editor::style::icon::Script, -2);

//--------------------------------------------------------------------------------------
EnemyBehaviour::EnemyBehaviour(const string & _name, IObject * _parent) :
    super(_name, _parent, CharacterType::Enemy)
{
    EnableUpdateFlags(UpdateFlags::FixedUpdate | UpdateFlags::Update);
}

//--------------------------------------------------------------------------------------
EnemyBehaviour::~EnemyBehaviour()
{

}

//--------------------------------------------------------------------------------------
bool EnemyBehaviour::registerProperties(IClassDesc & _desc)
{
    super::registerProperties(_desc);

    registerProperty(EnemyBehaviour, m_detectionRadius, "Detection radius");
    setPropertyDescription(EnemyBehaviour, m_detectionRadius, "If a player is detected in this radius, it will be detected");

    registerProperty(EnemyBehaviour, m_attackRadius, "Punch radius");
    setPropertyDescription(EnemyBehaviour, m_attackRadius, "If a player is detected in this radius, it will be punched");
    
    registerPropertyGroupBegin(EnemyBehaviour, "Enemy debug");
    {
        registerPropertyEx(EnemyBehaviour, m_targetPosNew, "Target position", PropertyFlags::Transient);
        registerPropertyEx(EnemyBehaviour, m_targetPosSmooth, "Target position (smoothed)", PropertyFlags::Transient);
    }
    registerPropertyGroupEnd(EnemyBehaviour);

    return true;
}

//--------------------------------------------------------------------------------------
void EnemyBehaviour::OnPlay()
{
    super::OnPlay();
    const auto & options = Game::get()->Engine().GetOptions();
    m_playerTag = options->GetGameObjectTag("Player");
}

//--------------------------------------------------------------------------------------
void EnemyBehaviour::OnStop()
{
    super::OnStop();
    m_targetPosNew = 0;
    m_targetPosSmooth = 0;
}

//--------------------------------------------------------------------------------------
void EnemyBehaviour::OnTriggerEnter(IGameObject * _other)
{
    if (asBool(_other->GetTags() & m_playerTag))
    {
        VG_WARNING("[Enemy] Player \"%s\" is being punched by enemy \"%s\"", _other->GetName().c_str(), GetGameObject()->GetName().c_str());
        
        if (PlayerBehaviour * playerBehaviour = _other->GetComponentInChildrenT<PlayerBehaviour>())
        {
            playerBehaviour->TakeHit(this, this->GetGameObject()->GetComponentInChildrenT<WeaponBehaviour>());
        }
    }
}

//--------------------------------------------------------------------------------------
void EnemyBehaviour::FixedUpdate(const Context & _context)
{
    struct ActivePlayerInfo
    {
        PlayerBehaviour * behaviour = nullptr;
        float3 position = (float3)0.0f;
        float distance = 0.0f;
    };

    if (_context.m_playing && !_context.m_paused)   
    {
        if (MoveState::Die == m_moveState)
        {
            IAnimationComponent * animationComponent = _context.m_gameObject->GetComponentT<IAnimationComponent>();

            if (IAnimationResource * anim = animationComponent->GetAnimation(m_moveAnim[asInteger(MoveState::Die)]))
            {
                if (anim->GetTime() > 0.5f)
                {
                    if (auto * characterController = getGameObject()->GetComponentT<ICharacterControllerComponent>())
                        characterController->Enable(false);

                    auto bodies = getGameObject()->GetComponentsInChildrenT<IPhysicsBodyComponent>();
                    for (auto * body : bodies)
                        body->Enable(false);
                }
            }
        }
        else
        {
            const float3 pos = _context.m_gameObject->GetGlobalMatrix()[3].xyz;

            // Find nearest player (TODO: game->getActivePlayersInfos()?)
            Game * game = Game::get();
            const auto players = game->getPlayers();
            vector<ActivePlayerInfo> activePlayersInfos;
            for (auto * player : players)
            {
                if (player->isActive() && player->getHealth() > 0)
                {
                    auto & info = activePlayersInfos.emplace_back();
                    info.behaviour = player;
                    info.position = player->getGameObject()->GetGlobalMatrix()[3].xyz;
                    info.distance = length(info.position - pos);
                }
            }

            float3 translation = (float3)0.0f;

            ActivePlayerInfo closestPlayerInfo;

            if (activePlayersInfos.size() > 0)
            {
                // sort categories in alphabetical order
                sort(activePlayersInfos.begin(), activePlayersInfos.end(), [](ActivePlayerInfo & a, ActivePlayerInfo & b)
                    {
                        return a.distance < b.distance;
                    }
                );

                closestPlayerInfo = activePlayersInfos[0];
                vg::renderer::IDebugDraw * dbgDraw = Game::Engine().GetRenderer()->GetDebugDraw();

                const bool isClose = closestPlayerInfo.distance <= m_detectionRadius;
                const bool isVeryClose = closestPlayerInfo.distance <= m_attackRadius && m_moveState != MoveState::Hurt && m_moveState != MoveState::Die;

                float3 dir = (float3)0.0f;

                if (isVeryClose)
                {
                    if (m_fightState != FightState::Hit)
                    {
                        m_fightState = FightState::Hit;
                        playFightAnim(FightState::Hit, false);
                        playSound(SoundState::Hit);
                    }
                }

                if (isClose || isVeryClose)
                {
                    m_isActive = true;

                    if (!m_targetAcquired)
                    {
                        m_targetAcquired = true;
                        m_targetPosSmooth = pos;
                    }

                    m_targetPosNew = closestPlayerInfo.position;
                    m_targetPosSmooth = smoothdamp(m_targetPosSmooth, m_targetPosNew, m_targetPosSmoothdamp, 0.1f, _context.m_dt);

                    dbgDraw->AddLine(_context.m_world, pos, m_targetPosSmooth, 0x5FFFFFFF);
                    dbgDraw->AddLine(_context.m_world, pos, m_targetPosNew, 0xFF0000FF);
                }

                if (!isClose)
                {
                    m_moveState = MoveState::Idle;
                    m_targetAcquired = false;
                }

                m_currentSpeed = m_walkSpeed; // TODO : smooth

                if (MoveState::Hurt != m_moveState && FightState::Hit != m_fightState)
                {
                    if (m_targetAcquired)
                    {
                        dir = normalize(m_targetPosSmooth - pos);

                        if (m_fightState != FightState::Hit)
                            translation.xy = dir.xy * m_currentSpeed;

                        const float Yx = dir.x;
                        const float Xx = dir.y * (-1.0f);   // Do *NOT* write -dir.y because there's an issue with CLang and hlsl++ if you do so
                        const float alpha = atan2(Yx, Xx);

                        m_targetRotation = radiansToDegrees(alpha);
                    }

                    if (any(abs(translation.xy) > 0.0f))
                    {
                        if (m_currentSpeed >= (m_walkSpeed + m_runSpeed) * 0.5f)
                            m_moveState = MoveState::Run;
                        else
                            m_moveState = MoveState::Walk;
                    }
                }

                vg::engine::ICharacterControllerComponent * charaController = _context.m_gameObject->GetComponentT<vg::engine::ICharacterControllerComponent>();

                if (m_isActive && charaController)
                {
                    float3 currentVelocity = charaController->GetVelocity();
                    float3 targetVelocity = translation.xyz;
                    float3 updatedVelocity;
                    updatedVelocity.xy = smoothdamp(currentVelocity.xy, targetVelocity.xy, m_velocitySmoothdamp, 0.02f, _context.m_dt);
                    updatedVelocity.z = currentVelocity.z;

                    m_velocityNorm = (updatedVelocity.x != 0 || updatedVelocity.y != 0) ? (float)length(updatedVelocity) / _context.m_dt : 0.0f;

                    if (m_moveState != MoveState::Hurt)
                        charaController->SetVelocity(updatedVelocity);

                    m_currentRotation = smoothdamp(m_currentRotation, m_targetRotation, m_rotationSmoothdamp, 0.02f, _context.m_dt);

                    charaController->SetRotation(quaternion::rotation_z(degreesToRadians(m_currentRotation)));

                    //dbgDraw->AddLine(_context.m_world, pos, pos + normalize(updatedVelocity), 0xFF00FF00);
                }
            }   
            else
            {
                m_moveState = MoveState::Idle;
                m_targetAcquired = false;
            }
        }

        switch (m_moveState)
        {
            case MoveState::Idle:
                playMoveAnim(MoveState::Idle, true);
                break;

            case MoveState::Walk:
                playMoveAnim(MoveState::Walk, true);
                break;

            case MoveState::Run:
                playMoveAnim(MoveState::Run, true);
                break;

            case MoveState::Hurt:
                playMoveAnim(MoveState::Hurt, false);

                if (auto * animationComponent = _context.m_gameObject->GetComponentT<IAnimationComponent>())
                {
                    if (IAnimationResource * anim = animationComponent->GetAnimation(m_moveAnim[asInteger(MoveState::Hurt)]))
                    {
                        if (!anim->IsPlaying() || anim->IsFinished())
                            m_moveState = MoveState::Idle;
                    }
                }
                break;

            case MoveState::Die:
                playMoveAnim(MoveState::Die, false);
                break;
        }

        switch (m_fightState)
        {
            case FightState::None:
                break;

            case FightState::Hit:
            {
                if (auto * animationComponent = _context.m_gameObject->GetComponentT<IAnimationComponent>())
                {
                    if (IAnimationResource * anim = animationComponent->GetAnimation(m_fightAnim[asInteger(m_fightState)]))
                    {
                        if (!anim->IsPlaying() || anim->IsFinished())
                        {
                            stopFightAnim(m_fightState);
                            m_fightState = FightState::None;
                        }
                    }
                }
            }
            break;
        }
    }
}

//--------------------------------------------------------------------------------------
void EnemyBehaviour::Update(const Context & _context)
{
    super::Update(_context);
}