#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif

#include <dinput.h>
#include "core/Math/Math.h"

using namespace vg::core;

namespace vg::engine::win32
{
    static const core::u8 keycode[enumCount<core::Key>()] =
    {
        DIK_ESCAPE,		    // Key_ESCAPE
        DIK_1,			    // Key_1
        DIK_2,			    // Key_2
        DIK_3,			    // Key_3
        DIK_4,			    // Key_4
        DIK_5,			    // Key_5
        DIK_6,			    // Key_6
        DIK_7,			    // Key_7
        DIK_8,			    // Key_8
        DIK_9,			    // Key_9
        DIK_0,			    // Key_0
        DIK_MINUS,          // Key_MINUS
        DIK_EQUALS,		    // Key_EQUALS
        DIK_BACK,		    // Key_BACK
        DIK_TAB,		    // Key_TAB
        DIK_Q,			    // Key_Q
        DIK_W,			    // Key_W
        DIK_E,			    // Key_E
        DIK_R,			    // Key_R
        DIK_T,			    // Key_T
        DIK_Y,			    // Key_Y
        DIK_U,			    // Key_U
        DIK_I,			    // Key_I
        DIK_O,			    // Key_O
        DIK_P,			    // Key_P
        DIK_LBRACKET,       // Key_LBRACKET
        DIK_RBRACKET,       // Key_RBRACKET
        DIK_RETURN,         // Key_RETURN
        DIK_LCONTROL,       // Key_LCONTROL
        DIK_A,              // Key_A
        DIK_S,              // Key_S
        DIK_D,              // Key_D
        DIK_F,              // Key_F
        DIK_G,              // Key_G
        DIK_H,              // Key_H
        DIK_J,              // Key_J
        DIK_K,              // Key_K
        DIK_L,              // Key_L
        DIK_SEMICOLON,      // Key_SEMICOLON
        DIK_APOSTROPHE,     // Key_APOSTROPHE
        DIK_GRAVE,          // Key_GRAVE
        DIK_LSHIFT,         // Key_LSHIFT
        DIK_BACKSLASH,      // Key_BACKSLASH
        DIK_Z,              // Key_Z
        DIK_X,              // Key_X
        DIK_C,              // Key_C
        DIK_V,              // Key_V
        DIK_B,              // Key_B
        DIK_N,              // Key_N
        DIK_M,              // Key_M
        DIK_COMMA,          // Key_COMMA
        DIK_PERIOD,         // Key_PERIOD
        DIK_SLASH,          // Key_SLASH
        DIK_RSHIFT,         // Key_RSHIFT
        DIK_MULTIPLY,       // Key_MULTIPLY
        DIK_LMENU,          // Key_LMENU
        DIK_SPACE,          // Key_SPACE
        DIK_CAPITAL,        // Key_CAPITAL
        DIK_F1,             // Key_F1
        DIK_F2,             // Key_F2
        DIK_F3,             // Key_F3
        DIK_F4,             // Key_F4
        DIK_F5,             // Key_F5
        DIK_F6,             // Key_F6
        DIK_F7,             // Key_F7
        DIK_F8,             // Key_F8
        DIK_F9,             // Key_F9
        DIK_F10,            // Key_F10
        DIK_NUMLOCK,        // Key_NUMLOCK
        DIK_SCROLL,         // Key_SCROLL
        DIK_NUMPAD7,        // Key_NUMPAD7
        DIK_NUMPAD8,        // Key_NUMPAD8
        DIK_NUMPAD9,        // Key_NUMPAD9
        DIK_SUBTRACT,       // Key_SUBTRACT
        DIK_NUMPAD4,        // Key_NUMPAD4
        DIK_NUMPAD5,        // Key_NUMPAD5
        DIK_NUMPAD6,        // Key_NUMPAD6
        DIK_ADD,            // Key_ADD
        DIK_NUMPAD1,        // Key_NUMPAD1
        DIK_NUMPAD2,        // Key_NUMPAD2
        DIK_NUMPAD3,        // Key_NUMPAD3
        DIK_NUMPAD0,        // Key_NUMPAD0
        DIK_DECIMAL,        // Key_DECIMAL
        DIK_OEM_102,        // Key_OEM_102
        DIK_F11,            // Key_F11
        DIK_F12,            // Key_F12
        DIK_F13,            // Key_F13
        DIK_F14,            // Key_F14
        DIK_F15,            // Key_F15
        DIK_KANA,           // Key_KANA
        DIK_ABNT_C1,        // Key_ABNT_C1
        DIK_CONVERT,        // Key_CONVERT
        DIK_NOCONVERT,      // Key_NOCONVERT
        DIK_YEN,            // Key_YEN
        DIK_ABNT_C2,        // Key_ABNT_C2
        DIK_NUMPADEQUALS,   // Key_NUMPADEQUALS
        DIK_PREVTRACK,      // Key_PREVTRACK
        DIK_AT,             // Key_AT
        DIK_COLON,          // Key_COLON
        DIK_UNDERLINE,      // Key_UNDERLINE
        DIK_KANJI,          // Key_KANJI
        DIK_STOP,           // Key_STOP
        DIK_AX,             // Key_AX
        DIK_UNLABELED,      // Key_UNLABELED
        DIK_NEXTTRACK,      // Key_NEXTTRACK
        DIK_NUMPADENTER,    // Key_NUMPADENTER
        DIK_RCONTROL,       // Key_RCONTROL
        DIK_MUTE,           // Key_MUTE
        DIK_CALCULATOR,     // Key_CALCULATOR
        DIK_PLAYPAUSE,      // Key_PLAYPAUSE
        DIK_MEDIASTOP,      // Key_MEDIASTOP
        DIK_VOLUMEDOWN,     // Key_VOLUMEDOWN
        DIK_VOLUMEUP,       // Key_VOLUMEUP
        DIK_WEBHOME,        // Key_WEBHOME
        DIK_NUMPADCOMMA,    // Key_NUMPADCOMMA
        DIK_DIVIDE,         // Key_DIVIDE
        DIK_SYSRQ,          // Key_SYSRQ
        DIK_RMENU,          // Key_RMENU
        DIK_PAUSE,          // Key_PAUSE
        DIK_HOME,           // Key_HOME
        DIK_UP,             // Key_UP
        DIK_LEFT,           // Key_LEFT
        DIK_RIGHT,          // Key_RIGHT
        DIK_END,            // Key_END
        DIK_DOWN,           // Key_DOWN
        DIK_INSERT,         // Key_INSERT
        DIK_DELETE,         // Key_DELETE
        DIK_LWIN,           // Key_LWIN
        DIK_RWIN,           // Key_RWIN
        DIK_APPS,           // Key_APPS
        DIK_POWER,          // Key_POWER
        DIK_SLEEP,          // Key_SLEEP
        DIK_WAKE,           // Key_WAKE
        DIK_WEBSEARCH,      // Key_WEBSEARCH
        DIK_WEBFAVORITES,   // Key_WEBFAVORITES
        DIK_WEBREFRESH,     // Key_WEBREFRESH
        DIK_WEBSTOP,        // Key_WEBSTOP
        DIK_WEBFORWARD,     // Key_WEBFORWARD
        DIK_WEBBACK,        // Key_WEBBACK
        DIK_MYCOMPUTER,     // Key_MYCOMPUTER
        DIK_MAIL,           // Key_MAIL
        DIK_MEDIASELECT,    // Key_MEDIASELECT
        DIK_BACKSPACE,      // Key_BACKSPACE
        DIK_NUMPADSTAR,     // Key_NUMPADSTAR
        DIK_LALT,           // Key_LALT
        DIK_CAPSLOCK,       // Key_CAPSLOCK
        DIK_NUMPADMINUS,    // Key_NUMPADMINUS
        DIK_NUMPADPLUS,     // Key_NUMPADPLUS
        DIK_NUMPADPERIOD,   // Key_NUMPADPERIOD
        DIK_NUMPADSLASH,    // Key_NUMPADSLASH
        DIK_RALT,           // Key_RALT
        DIK_PGUP,           // Key_PGUP
        DIK_PGDN            // Key_PGDN
    };

    //--------------------------------------------------------------------------------------
    Input::Input(const vg::core::WinHandle & _appHandle) :
        super::Input(_appHandle)
    {
        HWND handle = (HWND)_appHandle;
        HRESULT hr;

        // init directInput
        {
            hr = DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&m_directInputDevice, nullptr);
            VG_ASSERT(SUCCEEDED(hr) && "Failed to initialize direct input manager");
        }

        // init keyboard
        {
            memset(&m_keyboardData, 0, sizeof(m_keyboardData));

            hr = m_directInputDevice->CreateDevice(GUID_SysKeyboard, &m_directInputKeyboard, nullptr);
            VG_ASSERT(SUCCEEDED(hr) && "Could not create keyboard input device");

            hr = m_directInputKeyboard->SetDataFormat(&c_dfDIKeyboard);
            VG_ASSERT(SUCCEEDED(hr) && "Error setting keyboard data format");

            hr = m_directInputKeyboard->SetCooperativeLevel(handle, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
            VG_ASSERT(SUCCEEDED(hr) && "Error setting keyboard cooperative level");

            hr = m_directInputKeyboard->Acquire();
        }

        // init mouse
        {
            memset(&m_mouseData, 0, sizeof(m_mouseData));

            m_directInputDevice->CreateDevice(GUID_SysMouse, &m_directInputMouse, nullptr);
            m_directInputMouse->SetDataFormat(&c_dfDIMouse);
            m_directInputMouse->SetCooperativeLevel(handle, DISCL_NONEXCLUSIVE);
        }

        initJoysticks();
    }

    //--------------------------------------------------------------------------------------
    void Input::OnPlay()
    {
        initJoysticks();
    }

    //--------------------------------------------------------------------------------------
    Input::~Input()
    {
        if (m_directInputKeyboard)
        {
            m_directInputKeyboard->Unacquire();
            m_directInputKeyboard->Release();
            m_directInputKeyboard = nullptr;
        }

        if (m_directInputMouse)
        {
            m_directInputMouse->Unacquire();
            m_directInputMouse->Release();
            m_directInputMouse = nullptr;
        }

        m_directInputDevice->Release();
        m_directInputDevice = nullptr;
    }

    //--------------------------------------------------------------------------------------
    bool Input::Update()
    {
        const auto handle = (HWND)m_appHandle;

        bool isFocused = GetActiveWindow() == handle;

        if (isFocused)
        {
            if (isFocused != m_wasFocused)
            {
                m_justFocused = true;
                //VG_DEBUGPRINT("[Input] m_justFocused = true;\n");
            }

            if (IsInputEnabled(InputType::Keyboard))
                UpdateKeyboard();

            if (IsInputEnabled(InputType::Mouse))
                UpdateMouse();
        }

        if (IsInputEnabled(InputType::Joypad))
            UpdateJoypads();

        m_wasFocused = isFocused;
        m_justFocused = false;
        
        return super::Update();
    }

#pragma region Mouse
    //--------------------------------------------------------------------------------------
    core::int3 Input::GetMouseDelta() const
    {
        return m_mouseData.m_posDelta;
    }

    //--------------------------------------------------------------------------------------
    core::uint2 Input::GetMousePos() const
    {
        return m_mouseData.m_pos;
    }

    //--------------------------------------------------------------------------------------
    bool Input::IsMouseButtonPressed(core::MouseButton _button) const
    {
        return 0 != m_mouseData.m_pressed[core::asInteger(_button)];
    }

    //--------------------------------------------------------------------------------------
    bool Input::wasMouseButtonPressed(core::MouseButton _button) const
    {
        return 0 != m_mouseData.m_wasPressed[core::asInteger(_button)];
    }

    //--------------------------------------------------------------------------------------
    bool Input::IsMouseButtonJustPressed(core::MouseButton _button) const
    {
        return IsMouseButtonPressed(_button) && !wasMouseButtonPressed(_button);
    }  

    //--------------------------------------------------------------------------------------
    bool Input::IsMouseButtonJustReleased(core::MouseButton _button) const
    {
        return !IsMouseButtonPressed(_button) && wasMouseButtonPressed(_button);
    }

    //--------------------------------------------------------------------------------------
    bool Input::IsMouseOverWindow() const
    {
        return m_mouseData.m_isOverWindow;
    }

    //--------------------------------------------------------------------------------------
    void Input::UpdateMouse()
    {
        VG_PROFILE_CPU("Mouse");

        const auto handle = (HWND)m_appHandle;

        DIMOUSESTATE ms;
        memset(&ms, 0x0, sizeof(ms));

        HRESULT hr = m_directInputMouse->GetDeviceState(sizeof(ms), &ms);
        if (FAILED(hr))
            m_directInputMouse->Acquire();

        const core::i32 threshold = 1000;

        POINT mousePos;
        GetCursorPos(&mousePos);

        RECT clientRect;
        GetClientRect(handle, &clientRect);

        RECT winRect;
        GetWindowRect(handle, &winRect);

        POINT p;
        p.x = 0;
        p.y = 0;
        ClientToScreen(handle, &p);

        if (mousePos.x - p.x >= clientRect.left && mousePos.x - p.x <= clientRect.right && mousePos.y - p.y >= clientRect.top && mousePos.y - p.y <= clientRect.bottom)
            m_mouseData.m_isOverWindow = true;
        else
            m_mouseData.m_isOverWindow = false;

        m_mouseData.m_pos.x = clamp(mousePos.x - p.x, 0L, clientRect.right - clientRect.left);
        m_mouseData.m_pos.y = clamp(mousePos.y - p.y, 0L, clientRect.bottom - clientRect.top);

        if (!m_justFocused)
        {
            m_mouseData.m_posDelta.x = abs(ms.lX) < threshold ? ms.lX : 0;
            m_mouseData.m_posDelta.y = abs(ms.lY) < threshold ? ms.lY : 0;
            m_mouseData.m_posDelta.z = abs(ms.lZ) < threshold ? ms.lZ : 0;
        }
        else
        {
            m_mouseData.m_posDelta = int3(0, 0, 0);
        }

        VG_STATIC_ASSERT(core::enumCount<core::MouseButton>() <= countof(ms.rgbButtons), "Invalid mouse button count");
        for (uint b = 0; b < core::enumCount<core::MouseButton>(); ++b)
        {
            if (!m_justFocused)
            {
                m_mouseData.m_wasPressed[b] = m_mouseData.m_pressed[b];
                m_mouseData.m_pressed[b] = 0 != ms.rgbButtons[b];
            }
            else
            {
                m_mouseData.m_wasPressed[b] = false;
                m_mouseData.m_pressed[b] = false;
            }
        }
    }
#pragma endregion Mouse

#pragma region Keyboard
    //--------------------------------------------------------------------------------------
    bool Input::IsKeyPressed(core::Key _key) const
    {
        return 0 != m_keyboardData.m_current[keycode[core::asInteger(_key)]];
    }

    //--------------------------------------------------------------------------------------
    bool Input::wasKeyPressed(core::Key _key) const
    {
        return 0 != m_keyboardData.m_previous[keycode[core::asInteger(_key)]];
    }

    //--------------------------------------------------------------------------------------
    bool Input::IsKeyJustPressed(core::Key _key) const
    {
        return IsKeyPressed(_key) && !wasKeyPressed(_key);
    }

    //--------------------------------------------------------------------------------------
    bool Input::IsKeyJustReleased(core::Key _key) const
    {
        return !IsKeyPressed(_key) && wasKeyPressed(_key);
    }

    //--------------------------------------------------------------------------------------
    void Input::UpdateKeyboard()
    {
        VG_PROFILE_CPU("Keyboard");

        memcpy(&m_keyboardData.m_previous, &m_keyboardData.m_current, m_keyboardData.s_buffersize);

        // Make sure we reacquire the keyboard if the device was lost 
        memset(&m_keyboardData.m_current, 0x0, sizeof(m_keyboardData.m_current));

        HRESULT hr = m_directInputKeyboard->GetDeviceState(m_keyboardData.s_buffersize, &m_keyboardData.m_current);
        if (FAILED(hr))
            m_directInputKeyboard->Acquire();
    }

#pragma endregion Keyboard

#pragma region Joy
    //--------------------------------------------------------------------------------------
    BOOL enumJoysticksCallback(LPCDIDEVICEINSTANCEA _instance, LPVOID _this)
    {
        Input * input = (Input*)_this;
        return input->detectJoystick(_instance);
    }

    //--------------------------------------------------------------------------------------
    BOOL setControllerRanges(LPCDIDEVICEOBJECTINSTANCEA _instance, LPVOID _controller)
    {
        // the game controller
        LPDIRECTINPUTDEVICE8 controller = (LPDIRECTINPUTDEVICE8)_controller;
        controller->Unacquire();

        DIPROPRANGE range;
                    range.lMin = -100;
                    range.lMax = 100;
                    range.diph.dwSize = sizeof(DIPROPRANGE);
                    range.diph.dwHeaderSize = sizeof(DIPROPHEADER);
                    range.diph.dwHow = DIPH_BYID;
                    range.diph.dwObj = _instance->dwType;

        // now set the range for the axis		
        HRESULT hr = controller->SetProperty(DIPROP_RANGE, &range.diph);
        VG_ASSERT(SUCCEEDED(hr));

        return DIENUM_CONTINUE;
    }

    //--------------------------------------------------------------------------------------
    void Input::initJoysticks()
    {
        m_directInputJoystick.clear();
        m_joystickData.clear();
        HRESULT hr = m_directInputDevice->EnumDevices(DI8DEVCLASS_GAMECTRL, &enumJoysticksCallback, this, DIEDFL_ATTACHEDONLY);
        VG_ASSERT(SUCCEEDED(hr) && "Could not enum joystick input devices");
    }

    //--------------------------------------------------------------------------------------
    core::uint Input::detectJoystick(LPCDIDEVICEINSTANCEA _instance)
    {
        IDirectInputDevice8A * controller = nullptr;

        HRESULT hr = m_directInputDevice->CreateDevice(_instance->guidInstance, &controller, nullptr);

        if (SUCCEEDED(hr))
        {
            hr = controller->SetDataFormat(&c_dfDIJoystick2);
            VG_ASSERT(SUCCEEDED(hr));

            // set range and dead zone of joystick axes
            hr = controller->EnumObjects(&setControllerRanges, controller, DIDFT_AXIS);
            VG_ASSERT(SUCCEEDED(hr));

            m_directInputJoystick.push_back(controller);

            JoystickData joyData = {};
            m_joystickData.push_back(joyData);
        }

        return DIENUM_CONTINUE;
    }

    //--------------------------------------------------------------------------------------
    void Input::UpdateJoypads()
    {
        VG_PROFILE_CPU("Joypads");

        VG_ASSERT(m_directInputJoystick.size() == m_joystickData.size());
        for (uint i = 0; i < m_directInputJoystick.size(); ++i)
        {
            HRESULT hr;

            DIJOYSTATE2 state;
            IDirectInputDevice8A * directInputJoy = m_directInputJoystick[i];

            if (FAILED(directInputJoy->Poll()))
                hr = directInputJoy->Acquire();

            hr = directInputJoy->GetDeviceState(sizeof(state), (void*)&state);
            
            JoystickData & joy = m_joystickData[i];

            if (SUCCEEDED(hr))
            {
                joy.m_leftStickDir.x = float(state.lX) / 100.0f;
                joy.m_leftStickDir.y = float(state.lY) / 100.0f;

                joy.m_rightStickDir.x = float(state.lRx) / 100.0f;
                joy.m_rightStickDir.y = float(state.lRy) / 100.0f;

                for (uint b = 0; b < enumCount<JoyButton>(); ++b)
                {
                    joy.m_wasPressed[b] = joy.m_pressed[b];

                    if (state.rgbButtons[b])
                        joy.m_pressed[b] = true;
                    else
                        joy.m_pressed[b] = false;
                }

                for (uint b = (uint)enumCount<JoyButton>(); b < 128; ++b)
                {
                    if (state.rgbButtons[b])
                        VG_WARNING("[Input] Unknown button %u pressed", b);
                }
            }
            else
            {
                VG_WARNING_ONCE("[Input] Could not get JoyPad %u state", i);

                joy.m_leftStickDir.x = 0.0f;
                joy.m_leftStickDir.y = 0.0f;

                joy.m_rightStickDir.x = 0.0f;
                joy.m_rightStickDir.y = 0.0f;

                for (uint b = 0; b < enumCount<JoyButton>(); ++b)
                    joy.m_pressed[b] = false;
            }
        }
    }

    //--------------------------------------------------------------------------------------
    core::uint Input::GetJoyCount() const
    {
        return (core::uint)m_joystickData.size();
    }

    //--------------------------------------------------------------------------------------
    core::float2 Input::GetJoyLeftStickDir(core::JoyID _id) const
    {
        return m_joystickData[_id].m_leftStickDir;
    }

    //--------------------------------------------------------------------------------------
    core::float2 Input::GetJoyRightStickDir(core::JoyID _id) const
    {
        return m_joystickData[_id].m_rightStickDir;
    }

    //--------------------------------------------------------------------------------------
    bool Input::IsJoyButtonPressed(core::JoyID _id, JoyButton _button) const
    {
        return m_joystickData[_id].m_pressed[core::asInteger(_button)];
    }

    //--------------------------------------------------------------------------------------
    bool Input::IsJoyButtonJustPressed(core::JoyID _id, JoyButton _button) const
    {
        return IsJoyButtonPressed(_id, _button) && !wasJoyButtonPressed(_id, _button);
    }

    //--------------------------------------------------------------------------------------
    bool Input::IsJoyButtonJustReleased(core::JoyID _id, JoyButton _button) const
    {
        return !IsJoyButtonPressed(_id, _button) && wasJoyButtonPressed(_id, _button);
    }

    //--------------------------------------------------------------------------------------
    bool Input::wasJoyButtonPressed(core::JoyID _id, JoyButton _button) const
    {
        return m_joystickData[_id].m_wasPressed[core::asInteger(_button)];
    }
#pragma endregion Joy
}