#include "Precomp.h"
#include "BallBehaviour.h"
#include "editor/Editor_Consts.h"
#include "Game.h"
#include "Behaviour/Character/Player/PlayerBehaviour.h"

using namespace vg::core;
using namespace vg::engine;

VG_REGISTER_COMPONENT_CLASS(BallBehaviour, "Ball", "Game", "A ball used to play soccer", vg::editor::style::icon::Script, 0);

//--------------------------------------------------------------------------------------
BallBehaviour::BallBehaviour(const string & _name, IObject * _parent) :
    super(_name, _parent)
{

}

//--------------------------------------------------------------------------------------
BallBehaviour::~BallBehaviour()
{

}

//--------------------------------------------------------------------------------------
bool BallBehaviour::registerProperties(IClassDesc & _desc)
{
    super::registerProperties(_desc);

    return true;
}

//--------------------------------------------------------------------------------------
void BallBehaviour::OnPlay()
{
    super::OnPlay();
    m_startPos = GetGameObject()->getGlobalMatrix()[3].xyz;
}

//--------------------------------------------------------------------------------------
// When player collides with the ball, then is become the new most recent player
//--------------------------------------------------------------------------------------
void BallBehaviour::OnCollisionEnter(vg::core::IGameObject * _other)
{
    if (auto * player = _other->GetComponentByType<PlayerBehaviour>())
    {
        if (player->isActive())
        {
            m_lastOwnerCharacter = player;
            GetGameObject()->SetColor(player->getCustomColor());
        }
    }
}

//--------------------------------------------------------------------------------------
void BallBehaviour::Update(const Context & _context)
{
    if (m_scored)
    {
        float curTime = Game::Engine().GetTime().m_enlapsedTimeSinceStartScaled;

        const float delay = 5.0f;
        if (curTime > m_scoredTime + delay)
        {
            VG_INFO("[Ball] Goal was scored %.0f seconds ago", delay);
            m_scored = false;
            m_scoredTime = -1.0f;
            _context.m_gameObject->SetColor(float4(1, 1, 1, 1));
        }
    }
}

//--------------------------------------------------------------------------------------
void BallBehaviour::onGoalScored(CharacterType _team)
{
    if (!m_scored)
    {
        switch (_team)
        {
        default:
            VG_ASSERT_ENUM_NOT_IMPLEMENTED(_team);
            break;

        case CharacterType::Neutral:
            break;

        case CharacterType::Player:
            if (m_lastOwnerCharacter)
                m_lastOwnerCharacter->addScore(-1); // CSC
            break;

        case CharacterType::Enemy:
            if (m_lastOwnerCharacter)
                m_lastOwnerCharacter->addScore(+1);
            break;
        }

        m_scored = true;
        m_scoredTime = Game::Engine().GetTime().m_enlapsedTimeSinceStartScaled;
    }
}
