#include "Precomp.h"
#include "Game.h"
#include "core/Kernel.h"
#include "core/Object/AutoRegisterClass.h"
#include "engine/IEngine.h"

#include "Behaviour/PlayerBehaviour.h"

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
    IProject("", nullptr)
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
bool Game::RegisterClasses()
{
    IFactory * factory = Kernel::getFactory();

    // Register classes to auto-register the "Engine" module
    AutoRegisterClassInfo::registerClasses(*factory);

    if (IClassDesc * desc = factory->registerPlugin(Game, "Game"))
        registerProperties(*desc);

    return true;
}

//--------------------------------------------------------------------------------------
bool Game::UnregisterClasses()
{
    IFactory * factory = Kernel::getFactory();
    return AutoRegisterClassInfo::unregisterClasses(*factory);
}

//--------------------------------------------------------------------------------------
// vg::core::IProject overrides
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
bool Game::init(vg::engine::IEngine & _engine, Singletons & _singletons)
{
    s_engine = &_engine;

    Kernel::setSingletons(_singletons);

    RegisterClasses();

    return true;
}

//--------------------------------------------------------------------------------------
bool Game::deinit()
{
    UnregisterClasses();

    return true;
}

//--------------------------------------------------------------------------------------
bool Game::update()
{
    return true;
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