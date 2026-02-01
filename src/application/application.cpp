#include "application/Precomp.h"
#include "application.h"
#include "engine/IEngine.h"
#include "core/IFactory.h"
#include "core/Kernel.h"
#include "core/Object/AutoRegisterClass.h"

using namespace vg::core;

namespace vg
{
    //--------------------------------------------------------------------------------------
    Application::Application()
    {

    }

    //--------------------------------------------------------------------------------------
    Application::~Application()
    {

    }

    //--------------------------------------------------------------------------------------
    void Application::Init(const ApplicationCreationParams & _creationParams, const core::Singletons & _singletons)
    {
        m_engine = _creationParams.engine;
        Kernel::setSingletons(_singletons);
        RegisterClasses();
    }

    //--------------------------------------------------------------------------------------
    void Application::Deinit()
    {

    }

    //--------------------------------------------------------------------------------------
    // Register classes to auto-register from the Application
    //--------------------------------------------------------------------------------------
    bool Application::RegisterClasses()
    {
        IFactory * factory = Kernel::getFactory();
        return AutoRegisterClassInfo::registerClasses(*factory);
    }

    //--------------------------------------------------------------------------------------
    bool Application::Update()
    {
        m_engine->RunOneFrame();

        return true;
    }
}