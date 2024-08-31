#include "engine/Precomp.h"
#include "Input.h"
#include "engine/Engine.h"

#ifdef WIN32
#include "engine/Input/win32/Input_win32.hpp"
#endif

#include "engine/Engine.h"
#include "renderer/IRenderer.h"
#include "renderer/IImGuiAdapter.h"

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

        const auto & engine = Engine::get();
        auto * mainWorld = engine->GetMainWorld();

        const auto & renderer = engine->GetRenderer();

        if (renderer)
        {
            const auto & imGuiAdapter = renderer->GetImGuiAdapter();

            bool anyViewFocused = false;

            for (uint j = 0; j < enumCount<gfx::ViewTarget>(); ++j)
            {
                const auto & views = renderer->GetViews((gfx::ViewTarget)j);
                for (uint i = 0; i < views.size(); ++i)
                {
                    const auto * view = views[i];
                    if (view && view->IsFocused())
                    {
                        anyViewFocused = true;
                        break;
                    }
                }
            }            

            if (nullptr != imGuiAdapter)
            {
                if (imGuiAdapter->IsKeyboardFocused() && !anyViewFocused && mainWorld && !mainWorld->IsPlaying())
                    EnableInput(InputType::Keyboard, false);
                else
                    EnableInput(InputType::Keyboard, true);

                if (imGuiAdapter->IsMouseFocused() && !anyViewFocused && mainWorld && !mainWorld->IsPlaying())
                    EnableInput(InputType::Mouse, false);
                else
                    EnableInput(InputType::Mouse, true);
            }
        }

        if (mainWorld && mainWorld->IsPlaying())
            EnableInput(InputType::Joypad, true);
        else
            EnableInput(InputType::Joypad, false);
        
        return super::Update();
    }
}