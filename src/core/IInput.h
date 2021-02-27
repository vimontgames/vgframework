#pragma once

namespace vg::core
{
    enum class MouseButton : core::u8;
    enum class Key : core::u16;
    enum class JoyButton : core::u8;

    using JoyID = core::u8;

    class IInput
    {
    public:
        virtual                 ~IInput                     () = default;
        
        virtual bool            update                      () = 0;

    #pragma region Mouse
        virtual core::int3      getMouseDelta               () const = 0;
        virtual core::uint2     getMousePos                 () const = 0;
        virtual bool            isMouseButtonPressed        (MouseButton _button) const = 0;
        virtual bool            isMouseButtonJustPressed    (MouseButton _button) const = 0;
        virtual bool            IsMouseButtonJustReleased   (MouseButton _button) const = 0;
        virtual bool            wasMouseButtonPressed       (MouseButton _button) const = 0;
        virtual bool            isMouseOverWindow           () const = 0;
    #pragma endregion Mouse

    #pragma region Keyboard
        virtual bool            isKeyPressed                (Key _key) const = 0;
        virtual bool            isKeyJustPressed            (Key _key) const = 0;
        virtual bool            wasKeyPressed               (Key _key) const = 0;
    #pragma endregion Keyboard

    #pragma region Joy
        virtual core::uint      getJoyCount                 () const = 0;
        virtual core::float2    getJoyDir                   (JoyID _id) const = 0;
        virtual bool            isJoyButtonPressed          (JoyID _id, JoyButton _button) const = 0;
        virtual bool            isJoyButtonJustPressed      (JoyID _id, JoyButton _button) const = 0;
        virtual bool            isJoyButtonJustReleased     (JoyID _id, JoyButton _button) const = 0;
        virtual bool            wasJoyButtonPressed         (JoyID _id, JoyButton _button) const = 0;
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
        Y
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