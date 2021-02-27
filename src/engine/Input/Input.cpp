#include "engine/Precomp.h"
#include "Input.h"

#ifdef WIN32
#include "engine/Input/win32/Input_win32.hpp"
#endif

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
        return super::update();
    }
}