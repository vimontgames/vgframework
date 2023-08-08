#pragma once

#include "editor/ImGui/Window/ImGuiWindow.h"

namespace vg::editor
{
    class ImGuiPlatform : public ImGuiWindow
    {
    public:
        ImGuiPlatform(const core::string & _name, Flags _flags) :
            ImGuiWindow(_name, _flags)
        {
        }

        virtual void DrawGUI() final;
    };
}