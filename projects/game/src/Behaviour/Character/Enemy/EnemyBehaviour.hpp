#include "Precomp.h"
#include "EnemyBehaviour.h"

using namespace vg::core;
using namespace vg::engine;

VG_REGISTER_COMPONENT_CLASS(EnemyBehaviour, "Enemy", "Game", "Player Behaviour", vg::editor::style::icon::Script, -2);

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

    registerPropertyGroupBegin(EnemyBehaviour, "Enemy");
    {
        registerPropertyEx(EnemyBehaviour, m_targetPosNew, "TargetPosNew", PropertyFlags::NotSaved);
        registerPropertyEx(EnemyBehaviour, m_targetPosSmooth, "TargetPosSmooth", PropertyFlags::NotSaved);
    }
    registerPropertyGroupEnd(EnemyBehaviour);

    return true;
}

//--------------------------------------------------------------------------------------
void EnemyBehaviour::OnPlay()
{
    super::OnPlay();
}

//--------------------------------------------------------------------------------------
void EnemyBehaviour::OnStop()
{
    super::OnStop();
    m_moveState = MoveState::Idle;
}

struct ActivePlayerInfo
{
    PlayerBehaviour * behaviour = nullptr;
    float3 position = (float3)0.0f;
    float distance = 0.0f;
};

//--------------------------------------------------------------------------------------
void EnemyBehaviour::FixedUpdate(const Context & _context)
{
    if (_context.m_world->IsPlaying() && !_context.m_world->IsPaused())   // TODO: use Context
    {
        if (MoveState::Die == m_moveState)
        {
            IAnimationComponent * animationComponent = _context.m_gameObject->GetComponentT<IAnimationComponent>();

            if (IAnimationResource * anim = animationComponent->GetAnimation(m_moveAnim[asInteger(MoveState::Die)]))
            {
                if (anim->GetTime() > 0.5f)
                {
                    if (auto * characterController = getGameObject()->GetComponentT<ICharacterControllerComponent>())
                        characterController->setComponentFlags(vg::core::ComponentFlags::Enabled, false);

                    auto bodies = getGameObject()->GetComponentsInChildrenT<IPhysicsBodyComponent>();
                    for (auto * body : bodies)
                        body->setComponentFlags(vg::core::ComponentFlags::Enabled, false);
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
                if (player->isActive())
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

                bool isClose = closestPlayerInfo.distance < 4.0f;

                //dbgDraw->AddLine(world, pos, closestPlayerInfo.position, m_isActive ? 0xFF0000FF : 0xFF00FF00);

                float3 dir = (float3)0.0f;

                if (isClose)
                {
                    m_isActive = true;

                    if (!m_targetAcquired)
                    {
                        m_targetAcquired = true;
                        m_targetPosSmooth = pos;
                    }

                    m_targetPosNew = closestPlayerInfo.position;
                    m_targetPosSmooth = smoothdamp(m_targetPosSmooth, m_targetPosNew, m_targetPosSmoothdamp, 0.1f, _context.m_dt);

                    dbgDraw->AddLine(_context.m_world, pos, m_targetPosSmooth, 0xFFFFFFFF);
                    dbgDraw->AddLine(_context.m_world, pos, m_targetPosNew, 0xFF0000FF);
                }
                else
                {
                    m_moveState = MoveState::Idle;
                    m_targetAcquired = false;
                }

                m_speedCurrent = m_walkSpeed; // TODO : smooth

                if (MoveState::Hurt != m_moveState)
                {
                    if (m_targetAcquired)
                    {
                        dir = normalize(m_targetPosSmooth - pos);
                        translation.xy = dir.xy * m_speedCurrent;
                        m_targetRotation = radiansToDegrees(atan2((float)dir.x, (float)-dir.y));
                    }

                    if (any(abs(translation.xy) > 0.0f))
                    {
                        if (m_speedCurrent >= (m_walkSpeed + m_runSpeed) * 0.5f)
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

                    dbgDraw->AddLine(_context.m_world, pos, pos + normalize(updatedVelocity), 0xFF00FF00);
                }
            }

            IAnimationComponent * animationComponent = _context.m_gameObject->GetComponentT<IAnimationComponent>();

            if (animationComponent)
            {
                if (MoveState::Hurt == m_moveState)
                {
                    if (IAnimationResource * anim = animationComponent->GetAnimation(m_moveAnim[asInteger(MoveState::Hurt)]))
                    {
                        if (anim->GetTime() > 1.2f)
                            m_moveState = MoveState::Idle;
                    }
                }
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
                break;

            case MoveState::Die:
                playMoveAnim(MoveState::Die, false);
                break;
        }
    }
}

//--------------------------------------------------------------------------------------
void EnemyBehaviour::Update(const Context & _context)
{
    super::Update(_context);
}