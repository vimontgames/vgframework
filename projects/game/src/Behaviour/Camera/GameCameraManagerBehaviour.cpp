#include "Precomp.h"
#include "GameCameraManagerBehaviour.h"
#include "editor/Editor_Consts.h"
#include "engine/ICameraComponent.h"
#include "GameCameraBehaviour.hpp"

using namespace vg::core;
using namespace vg::engine;

VG_REGISTER_COMPONENT_CLASS(GameCameraManagerBehaviour, "Game Camera Manager", "Game", "Camera Manager for Game cameras", vg::editor::style::icon::Script, 0);

//--------------------------------------------------------------------------------------
GameCameraManagerBehaviour::GameCameraManagerBehaviour(const string & _name, IObject * _parent) :
    super(_name, _parent)
{
    EnableUpdateFlags(UpdateFlags::Update);
}

//--------------------------------------------------------------------------------------
GameCameraManagerBehaviour::~GameCameraManagerBehaviour()
{

}

//--------------------------------------------------------------------------------------
bool GameCameraManagerBehaviour::registerProperties(IClassDesc & _desc)
{
    super::registerProperties(_desc);

    registerProperty(GameCameraManagerBehaviour, m_player1Camera, "Camera 1");
    registerProperty(GameCameraManagerBehaviour, m_player2Camera, "Camera 2");
    registerProperty(GameCameraManagerBehaviour, m_player3Camera, "Camera 3");
    registerProperty(GameCameraManagerBehaviour, m_player4Camera, "Camera 4");

    registerPropertyEx(GameCameraManagerBehaviour, m_players, "Players", vg::core::PropertyFlags::ReadOnly);
    setPropertyRange(GameCameraManagerBehaviour, m_players, uint2(0, 4));

    return true;
}

//--------------------------------------------------------------------------------------
void GameCameraManagerBehaviour::OnPropertyChanged(IObject * _object, const IProperty & _prop, bool _notifyParent)
{
    if (!strcmp(_prop.GetName(), "m_players"))
        updateGameViewports();

    super::OnPropertyChanged(_object, _prop, _notifyParent);
}

//--------------------------------------------------------------------------------------
void GameCameraManagerBehaviour::OnPlay()
{
    super::OnPlay();  
}

//--------------------------------------------------------------------------------------
void GameCameraManagerBehaviour::Update(const Context & _context)
{
    if (_context.m_playing && !_context.m_paused)
    {
        const uint playerCount = (uint)Game::get()->getActivePlayers().size();
        if (m_players != playerCount)
        {
            m_players = playerCount;
            updateGameViewports();
        }
    }
}

//--------------------------------------------------------------------------------------
void GameCameraManagerBehaviour::updateGameViewports()
{
    VG_INFO("[CameraManager] Update Game Viewports for %u players", m_players);

    vector<PlayerBehaviour *> activePlayersSorted = Game::get()->getActivePlayers();
    sort(activePlayersSorted.begin(), activePlayersSorted.end(), [](PlayerBehaviour * a, PlayerBehaviour * b)
    {
        return (a->getViewportIndex() < b->getViewportIndex());
    }
    );

    vector<IGameObject *> cameras(activePlayersSorted.size());

    for (uint i = 0; i < activePlayersSorted.size(); ++i)
    {
        const PlayerBehaviour * player = activePlayersSorted[i];
        IGameObject * camGO = nullptr; 

        const uint index = player->getViewportIndex() != -1 ? player->getViewportIndex() : i;
        switch (index)
        {
            default:
                VG_ASSERT_NOT_IMPLEMENTED();
                break;

            case 0:
                camGO = m_player1Camera.get<IGameObject>();
                break;

            case 1:
                camGO = m_player2Camera.get<IGameObject>();
                break;

            case 2:
                camGO = m_player3Camera.get<IGameObject>();
                break;

            case 3:
                camGO = m_player4Camera.get<IGameObject>();
                break;
        }

        cameras[i] = camGO;
        auto * playerGO = player->GetGameObject();
        camGO->SetName("Camera_" + playerGO->GetName());
        if (auto * gameCamBehaviour = camGO->GetComponentT<GameCameraBehaviour>())
            gameCamBehaviour->setTarget(playerGO);  
    }

    if (cameras.size() > 0)
    {
        if (m_players > 0)
        {
            cameras[0]->SetInstanceFlags(vg::core::InstanceFlags::Enabled, true);

            if (auto * cam0 = cameras[0]->GetComponentT<ICameraComponent>())
            {
                switch (m_players)
                {
                default:
                    VG_ASSERT_NOT_IMPLEMENTED();
                    break;

                case 0:
                case 1:                                                                             // ■■
                    cam0->SetViewportOffsetAndScale(float2(0.0f, 0.0f), float2(1.0f, 1.0f));        // ■■
                    break;

                case 2:
                    cam0->SetViewportOffsetAndScale(float2(0.0f, 0.0f), float2(0.5f, 1.0f));        // ■□
                    break;                                                                          // ■□

                case 3:
                case 4:                                                                             // ■□
                    cam0->SetViewportOffsetAndScale(float2(0.0f, 0.0f), float2(0.5f, 0.5f));        // □□
                    break;
                }
            }
        }
        else
        {
            cameras[0]->SetInstanceFlags(vg::core::InstanceFlags::Enabled, false);
        }
    }

    if (cameras.size() > 1)
    {
        if (m_players > 1)
        {
            cameras[1]->SetInstanceFlags(vg::core::InstanceFlags::Enabled, true);
            if (auto * cam1 = cameras[1]->GetComponentT<ICameraComponent>())
            {
                switch (m_players)
                {
                    default:
                        VG_ASSERT_NOT_IMPLEMENTED();
                        break;

                    case 2:
                        cam1->SetViewportOffsetAndScale(float2(0.5f, 0.0f), float2(0.5f, 1.0f));    // □■
                        break;                                                                      // □■

                    case 3:
                    case 4:                                                                         // □■
                        cam1->SetViewportOffsetAndScale(float2(0.5f, 0.0f), float2(0.5f, 0.5f));    // □□
                        break;
                }
            }
        }
        else
        {
            cameras[1]->SetInstanceFlags(vg::core::InstanceFlags::Enabled, false);
        }
    }

    if (cameras.size() > 2)
    {
        if (m_players > 2)
        {
            cameras[2]->SetInstanceFlags(vg::core::InstanceFlags::Enabled, true);
            if (auto * cam2 = cameras[2]->GetComponentT<ICameraComponent>())
            {
                switch (m_players)
                {
                    default:
                        VG_ASSERT_NOT_IMPLEMENTED();
                        break;

                    case 3:                                                                         // □□
                        cam2->SetViewportOffsetAndScale(float2(0.0f, 0.5f), float2(1.0f, 0.5f));    // ■■
                        break;

                    case 4:                                                                         // □□
                        cam2->SetViewportOffsetAndScale(float2(0.0f, 0.5f), float2(0.5f, 0.5f));    // ■□
                        break;
                }
            }
        }
        else
        {
            cameras[2]->SetInstanceFlags(vg::core::InstanceFlags::Enabled, false);
        }
    }

    if (cameras.size() > 3)
    {
        if (m_players > 3)
        {
            cameras[3]->SetInstanceFlags(vg::core::InstanceFlags::Enabled, true);
            if (auto * cam3 = cameras[3]->GetComponentT<ICameraComponent>())
            {
                switch (m_players)
                {
                    default:
                        VG_ASSERT_NOT_IMPLEMENTED();
                        break;

                    case 4:                                                                         // □□
                        cam3->SetViewportOffsetAndScale(float2(0.5f, 0.5f), float2(0.5f, 0.5f));    // □■
                        break;
                }
            }
        }
        else
        {
            cameras[3]->SetInstanceFlags(vg::core::InstanceFlags::Enabled, false);
        }
    }
}