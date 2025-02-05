#pragma once

namespace vg::core
{
    enum class MouseButton : core::u8;
    enum class Key : core::u16;
    enum class JoyButton : core::u8;

    using JoyID = core::u8;

    vg_enum_class(vg::core, InputType, u8,
        Keyboard = 0,
        Mouse,
        Joypad
    );

    vg_enum_class(vg::core, KeyboardLayout, u8,
        Qwerty = 0,
        Azerty
    );

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
        virtual KeyboardLayout  GetKeyboardLayout           () const = 0;
        virtual bool            IsKeyPressed                (Key _key) const = 0;
        virtual bool            IsKeyJustPressed            (Key _key) const = 0;
        virtual bool            IsKeyJustReleased           (Key _key) const = 0;
    #pragma endregion Keyboard

    #pragma region Joy
        virtual core::uint      GetJoyCount                 () const = 0;
        virtual core::float2    GetJoyLeftStickDir          (JoyID _id) const = 0;
        virtual core::float2    GetJoyRightStickDir         (JoyID _id) const = 0;
        virtual float           GetJoyLeftTrigger           (core::JoyID _id) const = 0;
        virtual float           GetJoyRightTrigger          (core::JoyID _id) const = 0;
        virtual bool            IsJoyButtonPressed          (JoyID _id, JoyButton _button) const = 0;
        virtual bool            IsJoyButtonJustPressed      (JoyID _id, JoyButton _button) const = 0;
        virtual bool            IsJoyButtonJustReleased     (JoyID _id, JoyButton _button) const = 0;
    #pragma endregion Joy
    };

    vg_enum_class(vg::core, MouseButton, u8,
        Left = 0,
        Right,
        Middle
    );

    vg_enum_class(vg::core, JoyButton, u8,
        A = 0,
        B,
        X,
        Y,
        LB,
        RB,
        Select,
        Start,
        LeftStick,
        RightStick
    );

    vg_enum_class(vg::core, Key, u16,
        Escape = 0,
        One,
        Two,
        Three,
        Four,
        Five,
        Six,
        Seven,
        Height,
        Nine,
        Zero,
        Minus,
        Equals,
        Back,
        Tab,
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
        LeftBracket,
        RightBracket,
        ReturnKey,
        LeftControl,
        A,
        S,
        D,
        F,
        G,
        H,
        J,
        K,
        L,
        SemiColon,
        Apostrophe,
        Grave,
        LeftShift,
        Backslash,
        Z,
        X,
        C,
        V,
        B,
        N,
        M,
        Comma,
        Period,
        Slash,
        RightShift,
        Multiply,
        LeftMenu,
        Space,
        Capital,
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
        NumLock,
        Scroll,
        NumPad7,
        NumPad8,
        NumPad9,
        Subtract,
        NumPad4,
        NumPad5,
        NumPad6,
        Add,
        NumPad1,
        NumPad2,
        NumPad3,
        NumPad0,
        Decimal,
        Oem102,
        F11,
        F12,
        F13,
        F14,
        F15,
        Kana,
        AbntC1,
        Convert,
        NoConvert,
        Yen,
        AbntC2,
        NumPadEquals,
        PrevTrack,
        At,
        Colon,
        Underline,
        Kanji,
        Stop,
        Ax,
        Unlabeled,
        NextTrack,
        NumPadEnter,
        RightControl,
        Mute,
        Calculator,
        PlayPause,
        MediaStop,
        VolumeDown,
        VolumeUp,
        WebHome,
        NumPadComma,
        Divide,
        SysRq,
        RMenu,
        Pause,
        Home,
        Up,
        Left,
        Right,
        End,
        Down,
        Insert,
        Del,
        LeftWin,
        RightWin,
        Apps,
        Power,
        Sleep,
        Wake,
        WebSearch,
        WebFavorites,
        WebRefresh,
        WebStop,
        WebForward,
        WebBack,
        MyComputer,
        Mail,
        MediaSelect,
        Backspace,
        NumPadStar,
        LeftAlt,
        CapsLock,
        NumPadMinus,
        NumPadPlus,
        NumPadPeriod,
        NumPadSlash,
        RightAlt,
        PageUp,
        PageDown
    );
}