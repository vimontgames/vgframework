#include "Precomp.h"
#include "Game.h"
#include "core/Kernel.h"
#include "core/Object/AutoRegisterClass.h"
#include "engine/IEngine.h"
#include "Behaviour/Character/Player/PlayerBehaviour.h"
#include "Behaviour/Item/ItemBehaviour.h"

using namespace vg::core;

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
void Game::Update(float _dt)
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
void Game::addCharacter(CharacterType _type, CharacterBehaviour * _character)
{
    auto & characters = m_characters[vg::core::asInteger(_type)];
    VG_ASSERT(!vector_helper::exists(characters, _character));
    characters.push_back(_character);
    VG_INFO("[Game] Add %s Character \"%s\"", asString(_type).c_str(), _character->GetName().c_str());
}

//--------------------------------------------------------------------------------------
void Game::removeCharacter(CharacterType _type, CharacterBehaviour * _character)
{
    auto & characters = m_characters[vg::core::asInteger(_type)];
    VG_ASSERT(vector_helper::exists(characters, _character));
    vector_helper::remove(characters, _character);
    VG_INFO("[Game] Remove %s Character \"%s\"", asString(_type).c_str(), _character->GetName().c_str());
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
void Game::addItem(ItemType _type, ItemBehaviour * _item)
{
    auto & items = m_items[vg::core::asInteger(_type)];
    VG_ASSERT(!vector_helper::exists(items, _item));
    items.push_back(_item);
    VG_INFO("[Game] Add %s Item \"%s\"", asString(_type).c_str(), _item->GetName().c_str());
}

//--------------------------------------------------------------------------------------
void Game::removeItem(ItemType _type, ItemBehaviour * _item)
{
    auto & items = m_items[vg::core::asInteger(_type)];
    VG_ASSERT(vector_helper::exists(items, _item));
    vector_helper::remove(items, _item);
    VG_INFO("[Game] Remove %s Item \"%s\"", asString(_type).c_str(), _item->GetName().c_str());
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