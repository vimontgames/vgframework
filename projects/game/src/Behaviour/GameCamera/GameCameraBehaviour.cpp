#include "Precomp.h"
#include "GameCameraBehaviour.h"
#include "Game.h"
#include "core/GameObject/GameObject.h"
#include "Behaviour/Player/PlayerBehaviour.h"
#include "editor/Editor_Consts.h"

using namespace vg::core;
using namespace vg::engine;

VG_REGISTER_COMPONENT_CLASS(GameCameraBehaviour, "Game Camera Behaviour", "Game", "Camera Behaviour following the average of player positions", vg::editor::style::icon::Script);

//--------------------------------------------------------------------------------------
GameCameraBehaviour::GameCameraBehaviour(const string & _name, IObject * _parent) :
    super(_name, _parent)
{
    SetUpdateFlags(UpdateFlags::Update);
}

//--------------------------------------------------------------------------------------
GameCameraBehaviour::~GameCameraBehaviour()
{

}

//--------------------------------------------------------------------------------------
bool GameCameraBehaviour::registerProperties(IClassDesc & _desc)
{
    super::registerProperties(_desc);

    registerProperty(GameCameraBehaviour, m_speed, "Speed");

    return true;
}

//--------------------------------------------------------------------------------------
void GameCameraBehaviour::OnPlay()
{
    super::OnPlay();
    m_offset = getGameObject()->getGlobalMatrix()[3].xyz;
}

//--------------------------------------------------------------------------------------
// Camera position is the average of player positions + original camera offset
//--------------------------------------------------------------------------------------
void GameCameraBehaviour::Update(float _dt)
{
    const auto players = Game::get()->getPlayers();    
    
    if (players.size() > 0)
    {
        auto matrix = getGameObject()->getGlobalMatrix();

        float3 avgPos = (float3)0.0f;
        for (uint i = 0; i < players.size(); ++i)
            avgPos.xyz += players[i]->getGameObject()->getGlobalMatrix()[3].xyz;

        m_target.xy = avgPos.xy / (float)players.size() + m_offset.xy;

        matrix[3].xy = smoothdamp(matrix[3].xy, m_target.xy, (float2&)m_targetVelocity.xy, m_speed, _dt);

        getGameObject()->setGlobalMatrix(matrix);
    }
}