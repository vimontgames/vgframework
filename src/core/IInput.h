#pragma once

namespace vg::core
{
    enum class MouseButton : core::u8;
    enum class Key : core::u16;
    enum class JoyButton : core::u8;

    using JoyID = core::u8;

    enum class InputType : core::u8
    {
        Keyboard = 0,
        Mouse,
        Joypad
    };

    class IInput
    {
    public:
        virtual                 ~IInput                     () = default;

        virtual void            OnPlay                      () = 0;

        virtual void            EnableInput                 (InputType _inputType, bool _enabled) = 0;
        virtual bool            IsInputEnabled              (InputType _inputType) const = 0;
        
        virtual bool            Update                      () = 0;
        virtual void            UpdateKeyboard              () = 0;
        virtual void            UpdateMouse                 () = 0;
        virtual void            UpdateJoypads               () = 0;

    #pragma region Mouse
        virtual core::int3      GetMouseDelta               () const = 0;
        virtual core::uint2     GetMousePos                 () const = 0;
        virtual bool            IsMouseButtonPressed        (MouseButton _button) const = 0;
        virtual bool            IsMouseButtonJustPressed    (MouseButton _button) const = 0;
        virtual bool            IsMouseButtonJustReleased   (MouseButton _button) const = 0;
        virtual bool            IsMouseOverWindow           () const = 0;
    #pragma endregion Mouse

    #pragma region Keyboard
        virtual bool            IsKeyPressed                (Key _key) const = 0;
        virtual bool            IsKeyJustPressed            (Key _key) const = 0;
        virtual bool            IsKeyJustReleased           (Key _key) const = 0;
    #pragma endregion Keyboard

    #pragma region Joy
        virtual core::uint      GetJoyCount                 () const = 0;
        virtual core::float2    GetJoyLeftStickDir          (JoyID _id) const = 0;
        virtual core::float2    GetJoyRightStickDir         (JoyID _id) const = 0;
        virtual bool            IsJoyButtonPressed          (JoyID _id, JoyButton _button) const = 0;
        virtual bool            IsJoyButtonJustPressed      (JoyID _id, JoyButton _button) const = 0;
        virtual bool            IsJoyButtonJustReleased     (JoyID _id, JoyButton _button) const = 0;
    #pragma endregion Joy
    };

    enum class MouseButton : core::u8
    {
        Left = 0,
        Right,
        Middle
    };

    enum class JoyButton : core::u8
    {
        A = 0,
        B,
        X,
        Y,
        LT,
        RT,
        Select,
        Start,
        LeftStick,
        RightStick
    };

    enum class Key : core::u16
    {
        ESCAPE = 0,
        ONE,
        TWO,
        THREE,
        FOUR,
        FIVE,
        SIX,
        SEVEN,
        HEIGHT,
        NINE,
        ZERO,
        MINUS,
        EQUALS,
        BACK,
        TAB,
        Q,
        W,
        E,
        R,
        T,
        Y,
        U,
        I,
        O,
        P,
        LBRACKET,
        RBRACKET,
        RETURN,
        LCONTROL,
        A,
        S,
        D,
        F,
        G,
        H,
        J,
        K,
        L,
        SEMICOLON,
        APOSTROPHE,
        GRAVE,
        LSHIFT,
        BACKSLASH,
        Z,
        X,
        C,
        V,
        B,
        N,
        M,
        Comma,
        PERIOD,
        SLASH,
        RSHIFT,
        MULTIPLY,
        LMENU,
        SPACE,
        CAPITAL,
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        NUMLOCK,
        SCROLL,
        NUMPAD7,
        NUMPAD8,
        NUMPAD9,
        SUBTRACT,
        NUMPAD4,
        NUMPAD5,
        NUMPAD6,
        ADD,
        NUMPAD1,
        NUMPAD2,
        NUMPAD3,
        NUMPAD0,
        DECIMAL,
        OEM_102,
        F11,
        F12,
        F13,
        F14,
        F15,
        KANA,
        ABNT_C1,
        CONVERT,
        NOCONVERT,
        YEN,
        ABNT_C2,
        NUMPADEQUALS,
        PREVTRACK,
        AT,
        COLON,
        UNDERLINE,
        KANJI,
        STOP,
        AX,
        UNLABELED,
        NEXTTRACK,
        NUMPADENTER,
        RCONTROL,
        MUTE,
        CALCULATOR,
        PLAYPAUSE,
        MEDIASTOP,
        VOLUMEDOWN,
        VOLUMEUP,
        WEBHOME,
        NUMPADCOMMA,
        DIVIDE,
        SYSRQ,
        RMENU,
        PAUSE,
        HOME,
        UP,
        LEFT,
        RIGHT,
        END,
        DOWN,
        INSERT,
        DEL,
        LWIN,
        RWIN,
        APPS,
        POWER,
        SLEEP,
        WAKE,
        WEBSEARCH,
        WEBFAVORITES,
        WEBREFRESH,
        WEBSTOP,
        WEBFORWARD,
        WEBBACK,
        MYCOMPUTER,
        MAIL,
        MEDIASELECT,
        BACKSPACE,
        NUMPADSTAR,
        LALT,
        CAPSLOCK,
        NUMPADMINUS,
        NUMPADPLUS,
        NUMPADPERIOD,
        NUMPADSLASH,
        RALT,
        PAGEUP,
        PAGEDOWN
    };
}