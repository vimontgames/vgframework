#pragma once

#include "ImguiEditor.h"

namespace vg::graphics::renderer
{
    class ImguiPlatform : public ImguiEditor
    {
    public:
        ImguiPlatform(const string & _name, Flags _flags) :
            ImguiEditor(_name, _flags)
        {
        }

        virtual void    display() final;
    };
}