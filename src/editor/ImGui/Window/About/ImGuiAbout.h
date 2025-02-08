#pragma once

#include "editor/ImGui/Window/ImGuiWindow.h"

namespace vg::editor
{
    struct LibraryDescription;

    class ImGuiAbout : public ImGuiWindow
    {
    public:
        ImGuiAbout();

        virtual void DrawGUI() final override;

    private:
        void drawLibraryDescriptionList(const char * _label, const char * _column0Label, const char * _column1Label, LibraryDescription * _infos, core::uint _count);
    };
}