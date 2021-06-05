#include "engine/Precomp.h"
#include "Input.h"

#ifdef WIN32
#include "engine/Input/win32/Input_win32.hpp"
#endif

#include "engine/Engine.h"
#include "graphics/renderer/IRenderer.h"
#include "graphics/renderer/IImmediateGUI.h"

namespace vg::engine
{
    namespace base
    {
        //--------------------------------------------------------------------------------------
        Input::Input(const core::WinHandle & _appHandle) :
            m_appHandle(_appHandle)
        {

        }

        //--------------------------------------------------------------------------------------
        Input::~Input()
        {

        }

        //--------------------------------------------------------------------------------------
        bool Input::update()
        {
            return true;
        }
    }

    //--------------------------------------------------------------------------------------
    Input::Input(const core::WinHandle & _appHandle) :
        super::Input(_appHandle)
    {

    }

    //--------------------------------------------------------------------------------------
    Input::~Input()
    {

    }

    //--------------------------------------------------------------------------------------
    bool Input::update()
    {
        VG_PROFILE_CPU("Input");

        if (Engine::get()->GetRenderer()->getImmediateGUI()->isFocused())
            return false;

        return super::update();
    }
}