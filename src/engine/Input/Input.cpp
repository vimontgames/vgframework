#include "engine/Precomp.h"
#include "Input.h"
#include "engine/Engine.h"

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
        void Input::EnableInput(InputType _input, bool _enable)
        {
            if (_enable)
                m_inputTypes |= 1 << (uint)_input;
            else
                m_inputTypes &= ~(1 << (uint)_input);
        }

        //--------------------------------------------------------------------------------------
        bool Input::IsInputEnabled(InputType _inputType) const
        {
            return 0 != (m_inputTypes & (1 << (uint)_inputType));
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
    bool Input::Update()
    {
        VG_PROFILE_CPU("Input");

        const auto & renderer = Engine::get()->GetRenderer();
        const auto & gui = renderer->getImmediateGUI();

        bool anyViewActive = false;

        for (uint j = 0; j < enumCount<graphics::driver::ViewType>(); ++j)
        {
            const auto & views = renderer->GetViews((graphics::driver::ViewType)j);
            for (uint i = 0; i < views.size(); ++i)
            {
                const auto * view = views[i];
                if (view && view->IsActive())
                {
                    anyViewActive = true;
                    break;
                }
            }
        }

        if (gui->IsKeyboardFocused() && !anyViewActive)
            EnableInput(InputType::Keyboard, false);
        else
            EnableInput(InputType::Keyboard, true);

        if (gui->IsMouseFocused() && !anyViewActive)
            EnableInput(InputType::Mouse, false);
        else
            EnableInput(InputType::Mouse, true);

        if (Engine::get()->isPlaying())
            EnableInput(InputType::Joypads, true);
        else
            EnableInput(InputType::Joypads, false);
        
        return super::Update();
    }
}