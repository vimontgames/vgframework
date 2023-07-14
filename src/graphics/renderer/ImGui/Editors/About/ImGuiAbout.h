#pragma once

#include "graphics/renderer/ImGui/Editors/ImguiEditor.h"

namespace vg::graphics::renderer
{
    class ImguiAbout : public ImguiEditor
    {
    public:
        ImguiAbout(const string & _name, Flags _flags) :
            ImguiEditor(_name, _flags)
        {
        }

        virtual void    display() final;
    };
}