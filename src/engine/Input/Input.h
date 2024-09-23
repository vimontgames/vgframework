#pragma once

#include "core/IInput.h"

namespace vg::engine
{
    struct MouseData
    {
        core::uint2 m_pos = core::uint2(0, 0);
        core::int3  m_posDelta = core::int3(0, 0, 0);
        bool        m_pressed[core::enumCount<core::MouseButton>()];
        bool        m_wasPressed[core::enumCount<core::MouseButton>()];
        bool        m_isOverWindow : 1;
    };

    struct KeyboardData
    {
        static const core::uint s_buffersize = 256;

        core::KeyboardLayout    m_layout = core::KeyboardLayout::Qwerty;
        char                    m_current[s_buffersize];
        char                    m_previous[s_buffersize];
    };

    struct JoystickData
    {
        core::float2    m_leftStickDir = core::uint2(0, 0);
        core::float2    m_rightStickDir = core::uint2(0, 0);
        float           m_leftTrigger = 0.0f;
        float           m_rightTrigger = 0.0f;
        bool            m_pressed[core::enumCount<core::JoyButton>()];
        bool            m_wasPressed[core::enumCount<core::JoyButton>()];
    };

    namespace base
    {
        class Input : public core::IInput
        {
        public:
                                        Input               (const core::WinHandle &_appHandle);
                                        ~Input              ();

            void                        EnableInput         (core::InputType _inputType, bool _enable) override;
            bool                        IsInputEnabled      (core::InputType _inputType) const override;

            bool                        Update              () { return true; };

        protected:
            const core::WinHandle &     m_appHandle;
            core::u8                    m_inputTypes = 0;
            MouseData                   m_mouseData;
            KeyboardData                m_keyboardData;
            core::vector<JoystickData>  m_joystickData;
        };
    }
}

#ifdef WIN32
#include "engine/Input/win32/Input_win32.h"
#define VG_PLATFORM win32
#endif

namespace vg::engine
{
    class Input final : public VG_PLATFORM::Input
    {
        using super = VG_PLATFORM::Input;

    public:
        Input(const vg::core::WinHandle & _appHandle);
        ~Input();

        bool Update() override;

    private:
    };
}