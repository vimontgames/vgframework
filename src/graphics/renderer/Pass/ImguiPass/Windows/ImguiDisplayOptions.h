#pragma once

#include "ImguiEditor.h"

namespace vg::graphics::renderer
{
    class ImguiDisplayOptions: public ImguiEditor
    {
    public:
        ImguiDisplayOptions(const string & _name, Flags _flags) :
            ImguiEditor(_name, _flags)
        {
        }

        virtual void    display() final;
    };
}