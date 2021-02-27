#pragma once

#include "engine/Input/Input.h"

struct IDirectInput8A;
struct IDirectInputDevice8A;

namespace vg::engine::win32
{
    class Input : public base::Input
    {
        using super = base::Input;

    public:
        Input(const vg::core::WinHandle & _appHandle);
        ~Input();

        bool            update                  () override;

        core::int3      getMouseDelta           () const override;
        core::uint2     getMousePos             () const override;

        bool            isButtonPressed         (core::MouseButton _button) const override;
        bool            isButtonJustPressed     (core::MouseButton _button) const override;
        bool            IsButtonJustReleased    (core::MouseButton _button) const override;
        bool            wasButtonPressed        (core::MouseButton _button) const override;

        bool            isMouseOverWindow       () const override;

        bool            isKeyPressed            (core::Key _key) const override;
        bool            isKeyJustPressed        (core::Key _key) const override;
        bool            wasKeyPressed           (core::Key _key) const override;

    private:
        IDirectInput8A *			m_directInputDevice   = nullptr;
        IDirectInputDevice8A *		m_directInputKeyboard = nullptr;
        IDirectInputDevice8A *		m_directInputMouse    = nullptr;
    };
}