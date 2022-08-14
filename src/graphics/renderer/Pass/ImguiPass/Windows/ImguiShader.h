#pragma once

#include "ImguiEditor.h"

namespace vg::graphics::renderer
{
    class ImguiShader : public ImguiEditor
    {
    public:
        ImguiShader(const string & _name, Flags _flags) :
            ImguiEditor(_name, _flags)
        {
        }

        virtual void    display() final;
    };
}