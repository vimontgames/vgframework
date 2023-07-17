#include "Precomp.h"
#include "application.h"
#include "engine/IEngine.h"

namespace vg
{
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
    bool Application::update()
    {
        m_engine.RunOneFrame();

        return true;
    }
}