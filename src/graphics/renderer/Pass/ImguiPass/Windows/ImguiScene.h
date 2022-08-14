#pragma once

#include "ImguiEditor.h"

namespace vg::graphics::renderer
{
    class ImguiScene : public ImguiEditor
    {
    public:
        ImguiScene(const string & _name, Flags _flags) :
            ImguiEditor(_name, _flags)
        {
        }

        virtual void    display         () final;

    private:
        void            displaySector   (ISector * root);
    };
}