#include "BallBehaviour.h"
#include "Game.h"
#include "Behaviour/Entity/Character/Player/PlayerBehaviour.h"
#include "Behaviour/Entity/Character/Enemy/EnemyBehaviour.h"

using namespace vg::core;
using namespace vg::engine;

VG_REGISTER_COMPONENT_CLASS(BallBehaviour, "Ball", "Game", "A ball used to play soccer", vg::editor::style::icon::Script, 0);

//--------------------------------------------------------------------------------------
BallBehaviour::BallBehaviour(const string & _name, IObject * _parent) :
    super(_name, _parent, ItemType::Ball)
{
    EnableUpdateFlags(UpdateFlags::Update, true);
}

//--------------------------------------------------------------------------------------
BallBehaviour::~BallBehaviour()
{

}

//--------------------------------------------------------------------------------------
bool BallBehaviour::registerProperties(IClassDesc & _desc)
{
    super::registerProperties(_desc);

    registerPropertyEnum(BallBehaviour, BallType, m_ballType, "Type");
    setPropertyDescription(BallBehaviour, m_ballType, "Ball type");

    return true;
}

//--------------------------------------------------------------------------------------
void BallBehaviour::OnPlay()
{
    super::OnPlay();
    m_startPos = GetGameObject()->getGlobalMatrix()[3].xyz;
}

//--------------------------------------------------------------------------------------
void BallBehaviour::OnCollisionEnter(vg::core::IGameObject * _other)
{
    super::OnCollisionEnter(_other);
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
            if (auto * character = GetOwnerCharacter())
                character->addScore(-1); // CSC
            break;

        case CharacterType::Enemy:
            if (auto * character = GetOwnerCharacter())
                character->addScore(+1);
            break;
        }

        m_scored = true;
        m_scoredTime = Game::Engine().GetTime().m_enlapsedTimeSinceStartScaled;
    }
}
