#pragma once

#include "core/IInput.h"

namespace vg::engine
{
    struct MouseData
    {
        core::uint2 m_pos;
        core::int3  m_posDelta;
        bool        m_pressed[core::enumCount<core::MouseButton>()];
        bool        m_wasPressed[core::enumCount<core::MouseButton>()];
        bool        m_isOverWindow : 1;
    };

    struct KeyboardData
    {
        static const core::uint s_buffersize = 256;
        char        m_current[s_buffersize];
        char        m_previous[s_buffersize];
    };

    struct JoystickData
    {
        core::float2    m_dir;
        bool            m_pressed[core::enumCount<core::JoyButton>()];
        bool            m_wasPressed[core::enumCount<core::JoyButton>()];
    };

    namespace base
    {
        class Input : public core::IInput
        {
        public:
            Input(const core::WinHandle &_appHandle);
            ~Input();

            bool update() override;

        protected:
            const core::WinHandle &     m_appHandle;

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
    class Input : public VG_PLATFORM::Input
    {
        using super = VG_PLATFORM::Input;

    public:
        Input(const vg::core::WinHandle & _appHandle);
        ~Input();

        bool update() override;

    private:
    };
}