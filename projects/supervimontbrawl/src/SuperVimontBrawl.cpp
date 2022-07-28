#include "Precomp.h"
#include "SuperVimontBrawl.h"
#include "engine/IEngine.h"
#include "core/Kernel.h"
#include "core/Object/AutoRegisterClass.h"
#include "Entity/Player/PlayerEntity.h"

using namespace vg::core;

//--------------------------------------------------------------------------------------
SuperVimontBrawl * CreateNew()
{
    return new SuperVimontBrawl();
}

//--------------------------------------------------------------------------------------
SuperVimontBrawl::SuperVimontBrawl() : 
    IProject("", nullptr)
{
    
}

//--------------------------------------------------------------------------------------
SuperVimontBrawl::~SuperVimontBrawl()
{
    VG_SAFE_RELEASE(m_player);
}

//--------------------------------------------------------------------------------------
// vg::core::IPlugin overrides
//--------------------------------------------------------------------------------------

#define SVB_VERSION_MAJOR 0
#define SVB_VERSION_MINOR 0

//--------------------------------------------------------------------------------------
IPlugin::Version SuperVimontBrawl::getVersion() const
{
    return { SVB_VERSION_MAJOR, SVB_VERSION_MINOR };
}

//--------------------------------------------------------------------------------------
bool SuperVimontBrawl::registerClasses()
{
    IFactory * factory = Kernel::getFactory();

    // Register classes to auto-register the "Engine" module
    AutoRegisterClassInfo::registerClasses(*factory);

    if (IClassDesc * desc = factory->registerClassSingletonHelper(SuperVimontBrawl, "SuperVimontBrawl", IClassDesc::Flags::None))
        registerProperties(*desc);

    return true;
}

//--------------------------------------------------------------------------------------
bool SuperVimontBrawl::unregisterClasses()
{
    IFactory * factory = Kernel::getFactory();
    return AutoRegisterClassInfo::unregisterClasses(*factory);
}

//--------------------------------------------------------------------------------------
// vg::core::IProject overrides
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
bool SuperVimontBrawl::init(vg::engine::IEngine & _engine, Singletons & _singletons)
{
    m_engine = &_engine;

    Kernel::setSingletons(_singletons);

    registerClasses();

    return true;
}

//--------------------------------------------------------------------------------------
bool SuperVimontBrawl::deinit()
{
    unregisterClasses();

    return true;
}

//--------------------------------------------------------------------------------------
bool SuperVimontBrawl::update()
{
    m_engine->runOneFrame();

    return true;
}