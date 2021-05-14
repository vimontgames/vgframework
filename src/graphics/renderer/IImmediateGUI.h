#pragma once

namespace vg::graphics::renderer
{
    class IImmediateGUI
    {
    public:
        virtual ~IImmediateGUI() = default;
        virtual bool isFocused() const = 0;
    };
}