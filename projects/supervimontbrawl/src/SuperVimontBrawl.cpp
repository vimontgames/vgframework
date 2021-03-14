#include "Precomp.h"
#include "SuperVimontBrawl.h"
#include "engine/IEngine.h"
#include "core/Kernel.h"
#include "core/IObjectFactory.h"
#include "Entity/Player/PlayerEntity.h"

using namespace vg;

//--------------------------------------------------------------------------------------
SuperVimontBrawl * CreateNew()
{
    return new SuperVimontBrawl();
}

//--------------------------------------------------------------------------------------
SuperVimontBrawl::SuperVimontBrawl()
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
core::IPlugin::Version SuperVimontBrawl::getVersion() const
{
    return { SVB_VERSION_MAJOR, SVB_VERSION_MINOR };
}

//--------------------------------------------------------------------------------------
bool SuperVimontBrawl::registerClasses()
{
    core::IObjectFactory * factory = core::Kernel::getObjectFactory();

    return true;
}

//--------------------------------------------------------------------------------------
// vg::core::IProject overrides
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
bool SuperVimontBrawl::init(vg::engine::IEngine & _engine)
{
    m_engine = &_engine;

    return true;
}

//--------------------------------------------------------------------------------------
bool SuperVimontBrawl::deinit()
{

    return true;
}

//--------------------------------------------------------------------------------------
bool SuperVimontBrawl::update()
{
    m_engine->runOneFrame();

    return true;
}