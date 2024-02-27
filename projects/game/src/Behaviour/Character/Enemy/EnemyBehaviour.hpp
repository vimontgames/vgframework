#include "Precomp.h"
#include "EnemyBehaviour.h"
#include "Game.h"
#include "editor/Editor_Consts.h"
#include "core/GameObject/GameObject.h"
#include "core/IWorld.h"
#include "renderer/IRenderer.h"
#include "renderer/IDebugDraw.h"

using namespace vg::core;
using namespace vg::engine;

VG_REGISTER_COMPONENT_CLASS(EnemyBehaviour, "Enemy", "Game", "Player Behaviour", vg::editor::style::icon::Script);

//--------------------------------------------------------------------------------------
EnemyBehaviour::EnemyBehaviour(const string & _name, IObject * _parent) :
    super(_name, _parent)
{
    SetUpdateFlags(UpdateFlags::FixedUpdate | UpdateFlags::Update);
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

    }
    registerPropertyGroupEnd(EnemyBehaviour);

    return true;
}

//--------------------------------------------------------------------------------------
void EnemyBehaviour::OnPlay()
{
    super::OnPlay();
    Game::get()->addCharacter(CharacterType::Enemy, this);
}

//--------------------------------------------------------------------------------------
void EnemyBehaviour::OnStop()
{
    Game::get()->removeCharacter(CharacterType::Enemy, this);
    super::OnStop();
}

struct ActivePlayerInfo
{
    PlayerBehaviour * behaviour = nullptr;
    float3 position = (float3)0.0f;
    float distance = 0.0f;
};

//--------------------------------------------------------------------------------------
void EnemyBehaviour::FixedUpdate(float _dt)
{
    auto * world = GetGameObject()->GetWorld();

    if (world->IsPlaying() && !world->IsPaused())
    {
        const float3 pos = this->GetGameObject()->GetGlobalMatrix()[3].xyz;

        // Find nearest player (TODO: game->getActivePlayersInfos()?)
        Game * game = Game::get();
        const auto players = game->getPlayers();
        vector<ActivePlayerInfo> activePlayersInfos;
        for (auto * player : players)
        {
            if (player->isActive())
            {
                auto & info = activePlayersInfos.push_empty();
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

            dbgDraw->AddLine(world, pos, closestPlayerInfo.position, m_isActive ? 0xFF0000FF : 0xFF00FF00);

            float3 dir = normalize(closestPlayerInfo.position - pos);

            m_currentSpeed = m_walkSpeed; // TODO : smooth

            translation.xy += dir.xy * float2(1, 1) * m_currentSpeed;
            m_currentRotation = radiansToDegrees(atan2((float)dir.x, (float)-dir.y));

            if (isClose)
            {
                if (any(abs(translation.xy) > 0.0f))
                {
                    if (m_currentSpeed >= (m_walkSpeed + m_runSpeed) * 0.5f)
                        m_state = CharacterState::Running;
                    else
                        m_state = CharacterState::Walking;
                }

                m_isActive = true;
            }

            vg::engine::ICharacterControllerComponent * charaController = GetGameObject()->GetComponentByType<vg::engine::ICharacterControllerComponent>();

            if (m_isActive && charaController)
            {
                float3 currentVelocity = charaController->GetVelocity();
                float3 targetVelocity = translation.xyz;
                float3 updatedVelocity;
                updatedVelocity.xy = 0.75f * currentVelocity.xy + 0.25f * targetVelocity.xy;
                updatedVelocity.z = currentVelocity.z;

                //if (abs((float)currentVelocity.z) <= 0.0001f && m_state == CharacterState::Jumping)
                //    m_state = CharacterState::Idle;
                //
                //if (jump)
                //    updatedVelocity += float3(0, 0, running ? m_runJumpSpeed : m_jumpSpeed);

                charaController->SetVelocity(updatedVelocity);
                charaController->SetRotation(quaternion::rotation_z(degreesToRadians(m_currentRotation)));
            }
        }

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
    }
}

//--------------------------------------------------------------------------------------
void EnemyBehaviour::Update(float _dt)
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