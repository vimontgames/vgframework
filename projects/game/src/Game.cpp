#include "Precomp.h"
#include "Game.h"
#include "GameOptions.h"
#include "core/Kernel.h"
#include "core/IProfiler.h"
#include "core/Object/AutoRegisterClass.h"
#include "core/IBaseScene.h"
#include "engine/IEngine.h"
#include "engine/IVehicleComponent.h"
#include "Behaviour/Entity/Character/Player/PlayerBehaviour.h"
#include "Behaviour/Entity/Item/ItemBehaviour.h"
#include "Behaviour/HealthBar/HealthBarBehaviour.h"

using namespace vg;
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
IGameOptions * Game::GetOptions() const
{
    return GameOptions::get();
}

//--------------------------------------------------------------------------------------
bool Game::Init(vg::engine::IEngine & _engine, Singletons & _singletons)
{
    s_engine = &_engine;

    Timer::init();

    Kernel::setSingletons(_singletons);

    RegisterClasses();

    // Load game options
    GameOptions * gameOptions = new GameOptions("GameOptions", this);

    return true;
}

//--------------------------------------------------------------------------------------
bool Game::Deinit()
{
    // Release game options
    GameOptions * gameOptions = GameOptions::get();
    VG_SAFE_DELETE(gameOptions);

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

    initMainMenu();
}

//--------------------------------------------------------------------------------------
void Game::OnStop()
{
    for (uint i = 0; i < enumCount<CharacterType>(); ++i)
        m_characters[i].clear();

    for (uint i = 0; i < enumCount<ItemType>(); ++i)
        m_items[i].clear();

    m_vehicles.clear();

    m_gameState = GameState::MainMenu;
}

//--------------------------------------------------------------------------------------
void Game::FixedUpdate(float _dt)
{
    VG_PROFILE_CPU("Game");

    for (auto & vehicle : m_vehicles)
    {
        IGameObject * go = vehicle->GetGameObject();

        auto world = go->getGlobalMatrix();

        if (world[3].z < GameOptions::get()->getDeathHeight())
        {
            // Kill all passengers
            const uint count = vehicle->GetPassengerSlotCount();
            for (uint i = 0; i < count; ++i)
            {
                if (IGameObject * passenger = vehicle->GetPassengerSlotOwner(i))
                {
                    if (PlayerBehaviour * player = passenger->GetComponentT<PlayerBehaviour>())
                    {
                        player->exitVehicle(false);
                    }
                }
            }

            vehicle->Respawn(float3(0,0, GameOptions::get()->getRespawnHeight()));
        }
    }

    for (uint e = 0; e < enumCount<CharacterType>(); ++e)
    {
        const auto type = (CharacterType)e;
        for (auto & character : getCharacters(type))
        {
            IGameObject * go = character->GetGameObject();

            auto world = go->getGlobalMatrix();

            if (world[3].z < GameOptions::get()->getDeathHeight())
                character->OnDeath();
        }
    }
}

//--------------------------------------------------------------------------------------
IGameObject * Game::findMainMenu() const
{
    IGameObject * mainMenu = nullptr;
    if (IWorld * world = s_engine->GetMainWorld())
    {
        auto sceneCount = world->GetSceneCount(BaseSceneType::Scene);
        for (uint i = 0; i < sceneCount; ++i)
        {
            const IBaseScene * scene = world->GetScene(i, BaseSceneType::Scene);
            const IGameObject * root = scene->GetRoot();
            mainMenu = root->GetChildGameObject("Main Menu");
        }
    }
    return mainMenu;
}

//--------------------------------------------------------------------------------------
static const char * g_avatars[] =
{
    "Pablo",
    "Romeo",
    "Benualdo"
};

//--------------------------------------------------------------------------------------
void Game::initMainMenu()
{
    if (IGameObject * mainMenu = findMainMenu())
    {
        // Assign avatar to joystick by default
        const uint avatarCount = (uint)countof(g_avatars);
        const uint maxPlayerCount = avatarCount;

        m_playerInputs.clear();
        m_playerInputs.reserve(maxPlayerCount);
        for (uint i = 0; i < maxPlayerCount; ++i)
        {
            PlayerInputInfo info;
            info.joyIndex = i;
            info.avatarIndex = i;
            info.enabled = false;

            m_playerInputs.push_back(info);
        }
    }

    for (CharacterBehaviour * character : m_characters[asInteger(CharacterType::Enemy)])
    {
        for (IUITextComponent * text : character->GetGameObject()->GetComponentsInChildrenT<IUITextComponent>())
            text->Enable(false);

        for (IUIImageComponent * image : character->GetGameObject()->GetComponentsInChildrenT<IUIImageComponent>())
            image->Enable(false);
    }
}

//--------------------------------------------------------------------------------------
void Game::Update(float _dt)
{
    if (s_engine->IsPlaying())
    {
        // find main menu
        IGameObject * mainMenu = findMainMenu();        

        // Update main menu
        switch (m_gameState)
        {
            case GameState::MainMenu:
            {
                const uint avatarCount = (uint)countof(g_avatars);
                const uint maxPlayerCount = avatarCount;

                // find main menu
                if (mainMenu)
                {
                    // sure main menu is enabled
                    mainMenu->Enable(true);

                    IInput & input = Game::Input();

                    // Update character selection
                    for (uint i = 0; i < m_playerInputs.size(); ++i)
                    {
                        PlayerInputInfo & info = m_playerInputs[i];

                        if (i < input.GetJoyCount())
                        {
                            // Activate joy input if any direction is used
                            if (any(abs(input.GetJoyLeftStickDir(i)) > 0.5f))
                            {
                                if (!info.enabled)
                                {
                                    // find an available avatar for that slot                                    
                                    VG_ASSERT(avatarCount < 32);
                                    u32 availableAvatarMask = (1<< avatarCount)-1;

                                    for (uint a = 0; a < avatarCount; ++a)
                                    {
                                        if (m_playerInputs[a].enabled)
                                            availableAvatarMask &= ~(1UL << m_playerInputs[a].avatarIndex);
                                    }

                                    for (uint a = 0; a < avatarCount; ++a)
                                    {
                                        if (availableAvatarMask & (1UL << a))
                                        {
                                            info.avatarIndex = a;
                                            break;
                                        }
                                    }    

                                    info.enabled = true;
                                }
                            }

                            if (info.enabled)
                            {
                                int newAvatarIndex = info.avatarIndex;
                                float leftStickX = input.GetJoyLeftStickDir(i).x;

                                const auto current = Timer::getTick();

                                if (Timer::getEnlapsedTime(info.lastInputTicks, current) / 1000.0f > 0.25f)
                                {
                                    if (-leftStickX > 0.5f)
                                    {
                                        uint prevSlot = newAvatarIndex;

                                        for (int n = 1; n < avatarCount; ++n)
                                        {
                                            int prev = (newAvatarIndex - n + avatarCount) % (int)avatarCount;

                                            bool used = false;
                                            for (uint k = 0; k < maxPlayerCount; ++k)
                                            {
                                                if ((k != i) && (m_playerInputs[k].enabled) && (m_playerInputs[k].avatarIndex) == prev)
                                                    used = true;
                                            }

                                            if (!used)
                                            {
                                                prevSlot = prev;
                                                break;
                                            }
                                        }

                                        newAvatarIndex = prevSlot;
                                    }
                                    else if (leftStickX > 0.5f)
                                    {
                                        uint nextSlot = newAvatarIndex;

                                        for (int n = 1; n < avatarCount; ++n)
                                        {
                                            int next = (newAvatarIndex + n + avatarCount) % (int)avatarCount;

                                            bool used = false;
                                            for (uint k = 0; k < maxPlayerCount; ++k)
                                            {
                                                if ((k != i) && (m_playerInputs[k].enabled) && (m_playerInputs[k].avatarIndex) == next)
                                                used = true;
                                            }

                                            if (!used)
                                            {
                                                nextSlot = next;
                                                break;
                                            }
                                        }

                                        newAvatarIndex = nextSlot;
                                    }

                                    if (newAvatarIndex != info.avatarIndex)
                                    {
                                        info.avatarIndex = newAvatarIndex;
                                        info.lastInputTicks = Timer::getTick();
                                    }
                                }
                            }
                        }

                        // Update icons & texts
                        IGameObject * selectPlayerGO = mainMenu->GetChildGameObject(fmt::sprintf("Select Player %u", i + 1));
                        if (selectPlayerGO)
                        {
                            selectPlayerGO->Enable(true);

                            if (IUITextComponent * uiText = selectPlayerGO->GetComponentT<IUITextComponent>())
                            {
                                if (info.enabled)
                                {
                                    uiText->SetColor(float4(1, 1, 1, 1));
                                    uiText->SetText(fmt::sprintf("J%u: %s", i, g_avatars[m_playerInputs[i].avatarIndex]));
                                }
                                else
                                {
                                    uiText->SetColor(float4(1, 1, 1, 0.5f));
                                    uiText->SetText(fmt::sprintf("J%u:",i));
                                }
                                
                            }
                            // Disable all characters but the selected one by default
                            auto & avatars = selectPlayerGO->GetChildren();
                            for (uint a = 0; a < avatars.size(); ++a)
                            {
                                IGameObject * avatar = avatars[a];

                                if (IUIImageComponent * uiImage = avatar->GetComponentT<IUIImageComponent>())
                                {
                                    if (info.avatarIndex == a && info.enabled)
                                        uiImage->SetColor(float4(1, 1, 1, 1));
                                    else
                                        uiImage->SetColor(float4(1, 1, 1, 0.0f));
                                }
                            }
                        }
                    }

                    if (IGameObject * pressStart = mainMenu->GetScene()->GetRoot()->GetChildGameObject("Press Start"))
                    {
                        if (IUITextComponent * uiText = pressStart->GetComponentT<IUITextComponent>())
                        {
                            uint enabledPlayerCount = 0;
                            for (uint k = 0; k < maxPlayerCount; ++k)
                            {
                                if (m_playerInputs[k].enabled)
                                    enabledPlayerCount++;
                            }

                            if (enabledPlayerCount == 0)
                            {
                                uiText->SetText("Select Players");
                                uiText->SetColor(float4(1, 1, 1, 1.0f));
                            }
                            else
                            {
                                uiText->SetText(fmt::sprintf("Start (%uP)", enabledPlayerCount));
                                uiText->SetColor(float4(1, 1, 1, 1.0f));
                            }
                        }
                    }
                    
                    bool start = false;

                    for (uint i = 0; i < m_playerInputs.size(); ++i)
                    {
                        if (m_playerInputs[i].enabled)
                        {
                            if (input.IsJoyButtonPressed(i, JoyButton::Start))
                            {
                                // Assign controllers and start play
                                m_gameState = GameState::Playing;
                                mainMenu->Enable(false);
                                start = true;
                            }
                        }
                    };

                    if (start)
                    {
                        IGameObject * players = mainMenu->GetScene()->GetRoot()->GetChildGameObject("Players");

                        for (uint i = 0; i < m_playerInputs.size(); ++i)
                        {
                            auto & info = m_playerInputs[i];
                            if (info.enabled)
                            {
                                IGameObject * player = players->GetChildGameObject(g_avatars[info.avatarIndex]);
                                PlayerBehaviour * playerBehaviour = player->GetComponentInChildrenT<PlayerBehaviour>();
                                playerBehaviour->activate(info.joyIndex);
                            }
                        }

                        for (CharacterBehaviour * character : m_characters[asInteger(CharacterType::Enemy)])
                        {
                            for (IUITextComponent * text : character->GetGameObject()->GetComponentsInChildrenT<IUITextComponent>())
                                text->Enable(true);

                            for (IUIImageComponent * image : character->GetGameObject()->GetComponentsInChildrenT<IUIImageComponent>())
                                image->Enable(true);
                        }
                    }
                }
            }
            break;

            default:
                break;
        }
    }
}

//--------------------------------------------------------------------------------------
void Game::LateUpdate(float _dt)
{

}

//--------------------------------------------------------------------------------------
void Game::ToolUpdate(float _dt)
{

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