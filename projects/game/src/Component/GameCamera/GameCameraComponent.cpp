#include "Precomp.h"
#include "GameCameraComponent.h"
#include "Game.h"
#include "core/GameObject/GameObject.h"
#include "Behaviour/PlayerBehaviour.h"

using namespace vg::core;
using namespace vg::engine;

VG_REGISTER_COMPONENT_CLASS(GameCameraComponent, "GameCamera", "Game", "Game Camera");

//--------------------------------------------------------------------------------------
GameCameraComponent::GameCameraComponent(const string & _name, IObject * _parent) :
    super(_name, _parent)
{
    SetUpdateFlags(UpdateFlags::Update);
}

//--------------------------------------------------------------------------------------
GameCameraComponent::~GameCameraComponent()
{

}

//--------------------------------------------------------------------------------------
bool GameCameraComponent::registerProperties(IClassDesc & _desc)
{
    super::registerProperties(_desc);

    registerProperty(GameCameraComponent, m_speed, "Walk Speed");

    return true;
}

//--------------------------------------------------------------------------------------
void GameCameraComponent::OnPlay()
{
    super::OnPlay();
}

//--------------------------------------------------------------------------------------
void GameCameraComponent::Update(float _dt)
{
    const auto players = Game::get()->getPlayers();    
    
    if (players.size() > 0)
    {
        auto matrix = getGameObject()->getWorldMatrix();

        float3 avgPos = (float3)0.0f;
        for (uint i = 0; i < players.size(); ++i)
            avgPos.xyz += players[i]->getGameObject()->getWorldMatrix()[3].xyz;

        matrix[3].xy = avgPos.xy / (float)players.size() + float2(0,-18);
        //matrix[3].xyz = avgPos.xyz / (float)players.size() + float3(0, -18, 12);

        getGameObject()->setWorldMatrix(matrix);
    }
}