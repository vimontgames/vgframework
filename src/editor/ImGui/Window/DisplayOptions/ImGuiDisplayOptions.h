#pragma once

#include "editor/ImGui/Window/ImGuiWindow.h"

namespace vg::editor
{
    class ImGuiDisplayOptions: public ImGuiWindow
    {
    public:
        ImGuiDisplayOptions(const core::string & _name, Flags _flags) :
            ImGuiWindow(_name, _flags)
        {
        }

        virtual void DrawGUI() final override;
    };
}