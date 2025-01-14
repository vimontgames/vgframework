#include "GameCameraBehaviour.h"
#include "Game.h"
#include "core/GameObject/GameObject.h"
#include "Behaviour/Entity/Character/Player/PlayerBehaviour.h"

using namespace vg::core;
using namespace vg::engine;

VG_REGISTER_COMPONENT_CLASS(GameCameraBehaviour, "Game Camera", "Game", "Camera Behaviour following the average of player positions", vg::editor::style::icon::Script, 0);

//--------------------------------------------------------------------------------------
GameCameraBehaviour::GameCameraBehaviour(const string & _name, IObject * _parent) :
    super(_name, _parent)
{
    EnableUpdateFlags(UpdateFlags::Update);
}

//--------------------------------------------------------------------------------------
GameCameraBehaviour::~GameCameraBehaviour()
{

}

//--------------------------------------------------------------------------------------
bool GameCameraBehaviour::registerProperties(IClassDesc & _desc)
{
    super::registerProperties(_desc);

    registerProperty(GameCameraBehaviour, m_target, "Target");
    setPropertyDescription(GameCameraBehaviour, m_target, "Camera target");

    registerProperty(GameCameraBehaviour, m_offset, "Offset");
    setPropertyDescription(GameCameraBehaviour, m_offset, "Camera offset");

    registerProperty(GameCameraBehaviour, m_delay, "Delay");
    setPropertyDescription(GameCameraBehaviour, m_delay, "Time to smooth camera position (s)");

    return true;
}

//--------------------------------------------------------------------------------------
void GameCameraBehaviour::setTarget(const IGameObject * _target)
{
    m_target.setUID(_target->GetUID());
}

//--------------------------------------------------------------------------------------
void GameCameraBehaviour::OnEnable()
{
    super::OnEnable();
}

//--------------------------------------------------------------------------------------
// Camera position is the average of player positions + original camera offset
//--------------------------------------------------------------------------------------
void GameCameraBehaviour::Update(const Context & _context)
{
    if (_context.m_playing && !_context.m_paused)
    {
        auto * target = m_target.get<IGameObject>();
        vg::core::vector<CharacterBehaviour *> activePlayers;

        if (target)
        {
            if (auto * player = target->GetComponentInChildrenT<CharacterBehaviour>())
            {
                auto matrix = _context.m_gameObject->getGlobalMatrix();
                float3 pos = player->getGameObject()->getGlobalMatrix()[3].xyz;
                m_targetPosition.xyz = pos.xyz + m_offset.xyz;
                matrix[3].xyz = smoothdamp(matrix[3].xyz, m_targetPosition, m_targetVelocity, m_delay, _context.m_dt);
                _context.m_gameObject->setGlobalMatrix(matrix);
            }
        }
    }
}