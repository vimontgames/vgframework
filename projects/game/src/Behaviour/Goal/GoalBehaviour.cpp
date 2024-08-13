#include "Precomp.h"
#include "GoalBehaviour.h"
#include "editor/Editor_Consts.h"
#include "Game.h"
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

    registerPropertyEnum(GoalBehaviour, Team, m_team, "Team");
    setPropertyDescription(GoalBehaviour, m_team, "The team this goal belongs to");

    return true;
}

//--------------------------------------------------------------------------------------
void GoalBehaviour::OnEnable()
{
    super::OnEnable();
}

//--------------------------------------------------------------------------------------
void GoalBehaviour::Update(float _dt)
{
    
}

//--------------------------------------------------------------------------------------
void GoalBehaviour::OnTriggerEnter(IGameObject * _other)
{
    if (_other->getName()._Starts_with("Football"))
    {
        const auto & players = Game::get()->getPlayers();

        switch (m_team)
        {
            case Team::Humans:
            for (uint i = 0; i < players.size(); ++i)
            {
                auto * player = players[i];
                if (player->isActive())
                    player->addScore(-1);
            }
            _other->SetColor(float4(1, 0, 0, 1));
            break;

            case Team::Zombies:
            for (uint i = 0; i < players.size(); ++i)
            {
                auto * player = players[i];
                if (player->isActive())
                    player->addScore(+1);
            }
            _other->SetColor(float4(0, 1, 0, 1));
            break;
        }
    }
}