#pragma once

namespace vg::renderer
{
    class IImmediateGUI
    {
    public:
        virtual ~IImmediateGUI() = default;
        virtual bool IsKeyboardFocused() const = 0;
        virtual bool IsMouseFocused() const = 0;
    };
}