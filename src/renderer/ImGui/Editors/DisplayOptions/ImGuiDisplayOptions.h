#pragma once

#include "renderer/ImGui/Editors/ImguiEditor.h"

namespace vg::renderer
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