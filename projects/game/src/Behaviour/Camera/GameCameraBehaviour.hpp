#include "GameCameraBehaviour.h"
#include "Game.h"
#include "core/GameObject/GameObject.h"
#include "Behaviour/Entity/Character/Player/PlayerBehaviour.h"
#include "renderer/ICameraSettings.h"

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

    registerProperty(GameCameraBehaviour, m_delay, "Position Delay");
    setPropertyDescription(GameCameraBehaviour, m_delay, "Time to smooth camera position (s)");

    registerProperty(GameCameraBehaviour, m_speedToZoomCurve, "Speed/Zoom");
    setPropertyDescription(GameCameraBehaviour, m_speedToZoomCurve, "Zoom to speed curve");

    registerProperty(GameCameraBehaviour, m_zoomDelay, "Zoom Delay");

    registerPropertyEx(GameCameraBehaviour, m_currentSpeed, "Speed", vg::core::PropertyFlags::Transient);
    registerPropertyEx(GameCameraBehaviour, m_currentZoom, "Zoom", vg::core::PropertyFlags::Transient);

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

                if (ICameraComponent * cam = _context.m_gameObject->GetComponentT<ICameraComponent>())
                {
                    vg::renderer::ICameraSettings * camSettings = cam->GetCameraSettings();

                    m_currentSpeed = player->getVelocity();
                    
                    float zoom = m_speedToZoomCurve.getCurveValue(m_currentSpeed, 0);

                    m_currentZoom = smoothdamp(m_currentZoom, zoom, m_targetZoomVelocity, m_zoomDelay, _context.m_dt);

                    camSettings->SetZoomLevel(m_currentZoom);
                }
            }            
        }
    }
}