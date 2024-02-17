#pragma once

#include "editor/ImGui//Window/ImGuiWindow.h"

namespace vg::editor
{
    enum class Align
    {
        Left = 0,
        Center,
        Right
    };

    class ImGuiToolbar : public ImGuiWindow
    {
    public:
        ImGuiToolbar(const core::string & _name, Flags _flags);

        virtual void        DrawGUI             ();
        virtual void        DrawButtons         (Align _align) = 0;
        virtual core::uint  GetButtonCount      (Align _align) const = 0;

    protected:
        void                nextItem            ();
        
    private:
        ImGuiAxis           m_toolbarAxis       = ImGuiAxis_X;
    };
}