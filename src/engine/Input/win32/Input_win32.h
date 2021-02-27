#pragma once

#include "engine/Input/Input.h"

struct IDirectInput8A;
struct IDirectInputDevice8A;
struct DIDEVICEINSTANCEA;
typedef const DIDEVICEINSTANCEA *LPCDIDEVICEINSTANCEA;

namespace vg::engine::win32
{
    class Input : public base::Input
    {
        using super = base::Input;

    public:
        Input(const vg::core::WinHandle & _appHandle);
        ~Input();

        bool            update                      () override;

    #pragma region Mouse
        core::int3      getMouseDelta               () const override;
        core::uint2     getMousePos                 () const override;
        bool            isMouseButtonPressed        (core::MouseButton _button) const override;
        bool            isMouseButtonJustPressed    (core::MouseButton _button) const override;
        bool            IsMouseButtonJustReleased   (core::MouseButton _button) const override;
        bool            wasMouseButtonPressed       (core::MouseButton _button) const override;
        bool            isMouseOverWindow           () const override;
    #pragma endregion Mouse

    #pragma region Keyboard
        bool            isKeyPressed                (core::Key _key) const override;
        bool            isKeyJustPressed            (core::Key _key) const override;
        bool            wasKeyPressed               (core::Key _key) const override;
    #pragma endregion Keyboard

    #pragma region Joy
    public:
        core::uint      getJoyCount                 () const override;
        core::float2    getJoyDir                   (core::JoyID _id) const override;
        bool            isJoyButtonPressed          (core::JoyID _id, core::JoyButton _button) const override;
        bool            isJoyButtonJustPressed      (core::JoyID _id, core::JoyButton _button) const override;
        bool            isJoyButtonJustReleased     (core::JoyID _id, core::JoyButton _button) const override;
        bool            wasJoyButtonPressed         (core::JoyID _id, core::JoyButton _button) const override;

    private:
        void            initJoysticks               ();
        void            updateJoysticks             ();
        core::uint      detectJoystick              (LPCDIDEVICEINSTANCEA _instance);
        friend  BOOL    enumJoysticksCallback       (LPCDIDEVICEINSTANCEA _instance, LPVOID _this);
    #pragma endregion Joy

    private:
        IDirectInput8A *			            m_directInputDevice   = nullptr;
        IDirectInputDevice8A *		            m_directInputKeyboard = nullptr;
        IDirectInputDevice8A *		            m_directInputMouse    = nullptr;
        core::vector<IDirectInputDevice8A *>    m_directInputJoystick;
    };
}