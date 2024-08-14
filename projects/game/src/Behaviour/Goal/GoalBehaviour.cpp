#include "Precomp.h"
#include "GoalBehaviour.h"
#include "editor/Editor_Consts.h"
#include "Game.h"
#include "Behaviour/Ball/BallBehaviour.h"
#include "Behaviour/Character/Player/PlayerBehaviour.h"

using namespace vg::core;
using namespace vg::engine;

VG_REGISTER_COMPONENT_CLASS(GoalBehaviour, "Goal", "Game", "A goal where you can score", vg::editor::style::icon::Script, 0);

//--------------------------------------------------------------------------------------
GoalBehaviour::GoalBehaviour(const string & _name, IObject * _parent) :
    super(_name, _parent)
{
    SetUpdateFlags(UpdateFlags::Update);
}

//--------------------------------------------------------------------------------------
GoalBehaviour::~GoalBehaviour()
{

}

//--------------------------------------------------------------------------------------
bool GoalBehaviour::registerProperties(IClassDesc & _desc)
{
    super::registerProperties(_desc);

    registerPropertyEnum(GoalBehaviour, CharacterType, m_team, "Team");
    setPropertyDescription(GoalBehaviour, m_team, "The team this goal belongs to");

    return true;
}

//--------------------------------------------------------------------------------------
void GoalBehaviour::OnEnable()
{
    super::OnEnable();
}

//--------------------------------------------------------------------------------------
void GoalBehaviour::Update(const Context & _context)
{
    
}

//--------------------------------------------------------------------------------------
void GoalBehaviour::OnTriggerEnter(IGameObject * _other)
{
    if (auto * ball = _other->GetComponentByType<BallBehaviour>())
        ball->onGoalScored(m_team);
}