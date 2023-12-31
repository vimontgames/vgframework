#include "Precomp.h"
#include "application.h"
#include "engine/IEngine.h"

namespace vg
{
    #define VG_VERSION_MAJOR 0
    #define VG_VERSION_MINOR 23

    //--------------------------------------------------------------------------------------
    core::IPlugin::Version Application::GetVersion() const
    {
        return { VG_VERSION_MAJOR, VG_VERSION_MINOR };
    }

    //--------------------------------------------------------------------------------------
    Application::Application(vg::engine::IEngine & _engine) :
        m_engine(_engine)
    {

    }

    //--------------------------------------------------------------------------------------
    Application::~Application()
    {

    }

    //--------------------------------------------------------------------------------------
    bool Application::Update()
    {
        m_engine.RunOneFrame();

        return true;
    }
}