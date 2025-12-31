#include "Precomp.h"
#include "Game.h"
#include "core/Kernel.h"
#include "core/IProfiler.h"
#include "core/Object/AutoRegisterClass.h"
#include "core/IBaseScene.h"
#include "engine/IEngine.h"
#include "engine/IVehicleComponent.h"
#include "Behaviour/Entity/Character/Player/PlayerBehaviour.h"
#include "Behaviour/Entity/Item/ItemBehaviour.h"

using namespace vg::core;
using namespace vg::engine;

// Static member init
vg::engine::IEngine * Game::s_engine = nullptr;

//--------------------------------------------------------------------------------------
Game * CreateNew()
{
    return new Game();
}

//--------------------------------------------------------------------------------------
Game::Game() :
    IGame("", nullptr)
{

}

//--------------------------------------------------------------------------------------
Game::~Game()
{

}

//--------------------------------------------------------------------------------------
// vg::core::IPlugin overrides
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
bool Game::registerProperties(IClassDesc & _desc)
{
    super::registerProperties(_desc);

    return true;
}

//--------------------------------------------------------------------------------------
bool Game::RegisterClasses()
{
    IFactory * factory = Kernel::getFactory();

    // Register classes to auto-register from the "Game" module
    AutoRegisterClassInfo::registerClasses(*factory);

    if (IClassDesc * desc = factory->registerPlugin(Game, "Game"))
        registerProperties(*desc);

    return true;
}

//--------------------------------------------------------------------------------------
bool Game::UnregisterClasses()
{
    return AutoRegisterClassInfo::unregisterClasses();
}

//--------------------------------------------------------------------------------------
bool Game::Init(vg::engine::IEngine & _engine, Singletons & _singletons)
{
    s_engine = &_engine;

    Kernel::setSingletons(_singletons);

    RegisterClasses();

    return true;
}

//--------------------------------------------------------------------------------------
bool Game::Deinit()
{
    UnregisterClasses();

    return true;
}

//--------------------------------------------------------------------------------------
void Game::OnPlay()
{
    IWorld * world = s_engine->GetMainWorld();
    auto sceneCount = world->GetSceneCount(BaseSceneType::Scene);
    for (uint i = 0; i < sceneCount; ++i)
    {
        const IBaseScene * scene = world->GetScene(i, BaseSceneType::Scene);
        const IGameObject * root = scene->GetRoot();

        // cache characters
        auto allCharacters = root->GetComponentsByType("CharacterBehaviour", false, true); // search children too
        for (uint j = 0; j < allCharacters.size(); ++j)
        {
            CharacterBehaviour * character = (CharacterBehaviour * )allCharacters[j];
            m_characters[asInteger(character->getCharacterType())].push_back(character);
        }

        // cache items
        auto allItems = root->GetComponentsByType("ItemBehaviour", false, true); // search children too 
        for (uint j = 0; j < allItems.size(); ++j)
        {
            ItemBehaviour * item = (ItemBehaviour*)allItems[j];
            m_items[asInteger(item->getItemType())].push_back(item);
        }

        // cache vehicle list
        auto vehicles = root->GetComponentsByType("VehicleComponent", false, true); // search children too

        m_vehicles.reserve(vehicles.size());
        for (uint j = 0; j < vehicles.size(); ++j)
        {
            IVehicleComponent * vehicle = (IVehicleComponent*)vehicles[j];
            m_vehicles.push_back(vehicle);
        }
    }

    for (uint i = 0; i < enumCount<CharacterType>(); ++i)
        VG_INFO("[Game] %u CharacterBehaviour(s) of type \"%s\" found", m_characters[i].size(), asString((CharacterType)i).c_str());

    for (uint i = 0; i < enumCount<ItemType>(); ++i)
        VG_INFO("[Game] %u ItemBehaviour(s) of type \"%s\" found", m_items[i].size(), asString((ItemType)i).c_str());

    VG_INFO("[Game] %u VehicleComponent(s) found", m_vehicles.size());
}

//--------------------------------------------------------------------------------------
void Game::OnStop()
{
    for (uint i = 0; i < enumCount<CharacterType>(); ++i)
        m_characters[i].clear();

    for (uint i = 0; i < enumCount<ItemType>(); ++i)
        m_items[i].clear();

    m_vehicles.clear();
}

//--------------------------------------------------------------------------------------
void Game::FixedUpdate(float _dt)
{
    VG_PROFILE_CPU("Game");

    for (auto & vehicle : m_vehicles)
    {
        IGameObject * go = vehicle->GetGameObject();

        auto world = go->getGlobalMatrix();

        if (world[3].z < -32.0f)
        {
            // Kick all passengers
            const uint count = vehicle->GetPassengerSlotCount();
            for (uint i = 0; i < count; ++i)
            {
                if (IGameObject * passenger = vehicle->GetPassengerSlotOwner(i))
                {
                    if (PlayerBehaviour * player = passenger->GetComponentT<PlayerBehaviour>())
                        player->exitVehicle();
                }
            }

            vehicle->Respawn();
        }
    }
}

//--------------------------------------------------------------------------------------
vg::engine::IEngine & Game::Engine()
{
    VG_ASSERT(s_engine);
    return *s_engine;
}

//--------------------------------------------------------------------------------------
vg::core::IInput & Game::Input()
{
    auto * input = Kernel::getInput();
    VG_ASSERT(input);
    return *input;
}

//--------------------------------------------------------------------------------------
const vg::core::vector<PlayerBehaviour *> & Game::getPlayers() const 
{ 
    return reinterpret_cast<const vg::core::vector<PlayerBehaviour *> &>(m_characters[vg::core::asInteger(CharacterType::Player)]);
}

//--------------------------------------------------------------------------------------
const vg::core::vector<PlayerBehaviour *> Game::getActivePlayers() const
{
    const auto players = m_characters[vg::core::asInteger(CharacterType::Player)];
    vector<PlayerBehaviour *> activePlayers;
    activePlayers.reserve(players.size());
    for (auto * player : players)
    {
        if (player->isActive())
            activePlayers.push_back((PlayerBehaviour*)player);
    }
    return activePlayers;
}

//--------------------------------------------------------------------------------------
const vg::core::vector<EnemyBehaviour *> & Game::getEnemies() const 
{ 
    return reinterpret_cast<const vg::core::vector<EnemyBehaviour *> &>(m_characters[vg::core::asInteger(CharacterType::Player)]);
}

//--------------------------------------------------------------------------------------
const vg::core::vector<BallBehaviour *> & Game::getBalls() const
{
    return reinterpret_cast<const vg::core::vector<BallBehaviour *> &>(m_items[vg::core::asInteger(ItemType::Ball)]);
}

//--------------------------------------------------------------------------------------
const vg::core::vector<WeaponBehaviour *> & Game::getWeapons() const
{
    return reinterpret_cast<const vg::core::vector<WeaponBehaviour *> &>(m_items[vg::core::asInteger(ItemType::Weapon)]);
}

//--------------------------------------------------------------------------------------
const vg::core::vector<vg::engine::IVehicleComponent *> & Game::getVehicles() const
{
    return m_vehicles;
}