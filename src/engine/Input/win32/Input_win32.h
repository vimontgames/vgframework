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

        void            OnPlay                      () final override;

        bool            Update                      () override;

        void            UpdateKeyboard              () final override;
        void            UpdateMouse                 () final override;
        void            UpdateJoypads               () final override;

    #pragma region Mouse
    public:
        core::int3      GetMouseDelta               () const final override;
        core::uint2     GetMousePos                 () const final override;
        bool            IsMouseButtonPressed        (core::MouseButton _button) const final override;
        bool            IsMouseButtonJustPressed    (core::MouseButton _button) const final override;
        bool            IsMouseButtonJustReleased   (core::MouseButton _button) const final override;
        bool            IsMouseOverWindow           () const override;

    private:
        bool            wasMouseButtonPressed       (core::MouseButton _button) const;
    #pragma endregion Mouse

    #pragma region Keyboard
    public:
        bool            IsKeyPressed                (core::Key _key) const final override;
        bool            IsKeyJustPressed            (core::Key _key) const final override;
        bool            IsKeyJustReleased           (core::Key _key) const final override;

    private:
        bool            wasKeyPressed               (core::Key _key) const;
    #pragma endregion Keyboard

    #pragma region Joy
    public:
        core::uint      GetJoyCount                 () const override;
        core::float2    GetJoyLeftStickDir          (core::JoyID _id) const final override;
        core::float2    GetJoyRightStickDir         (core::JoyID _id) const final override;
        bool            IsJoyButtonPressed          (core::JoyID _id, core::JoyButton _button) const final override;
        bool            IsJoyButtonJustPressed      (core::JoyID _id, core::JoyButton _button) const final override;
        bool            IsJoyButtonJustReleased     (core::JoyID _id, core::JoyButton _button) const final override;

    private:
        bool            wasJoyButtonPressed         (core::JoyID _id, core::JoyButton _button) const;
        void            initJoysticks               ();
        core::uint      detectJoystick              (LPCDIDEVICEINSTANCEA _instance);
        friend  BOOL    enumJoysticksCallback       (LPCDIDEVICEINSTANCEA _instance, LPVOID _this);
    #pragma endregion Joy

    private:
        IDirectInput8A *			            m_directInputDevice   = nullptr;
        IDirectInputDevice8A *		            m_directInputKeyboard = nullptr;
        IDirectInputDevice8A *		            m_directInputMouse    = nullptr;
        core::vector<IDirectInputDevice8A *>    m_directInputJoystick;
        bool                                    m_wasFocused          = false;
        bool                                    m_justFocused         = false;
    };
}