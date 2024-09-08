#include "Precomp.h"
#include "Game.h"
#include "core/Kernel.h"
#include "core/Object/AutoRegisterClass.h"
#include "engine/IEngine.h"

#include "Behaviour/Character/Player/PlayerBehaviour.h"

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

#define GAME_VERSION_MAJOR 0
#define GAME_VERSION_MINOR 1

//--------------------------------------------------------------------------------------
IPlugin::Version Game::GetVersion() const
{
    return { GAME_VERSION_MAJOR, GAME_VERSION_MINOR };
}

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
}

//--------------------------------------------------------------------------------------
void Game::removeCharacter(CharacterType _type, CharacterBehaviour * _character)
{
    auto & characters = m_characters[vg::core::asInteger(_type)];
    VG_ASSERT(vector_helper::exists(characters, _character));
    vector_helper::remove(characters, _character);
}

//--------------------------------------------------------------------------------------
const vg::core::vector<PlayerBehaviour *> & Game::getPlayers() const 
{ 
    return reinterpret_cast<const vg::core::vector<PlayerBehaviour *> &>(m_characters[vg::core::asInteger(CharacterType::Player)]);
}
//--------------------------------------------------------------------------------------
const vg::core::vector<EnemyBehaviour *> & Game::getEnemies() const 
{ 
    return reinterpret_cast<const vg::core::vector<EnemyBehaviour *> &>(m_characters[vg::core::asInteger(CharacterType::Player)]);
}