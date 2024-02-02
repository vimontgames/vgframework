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

        static ImVec2       ComputeButtonSize   ();

    protected:
        void                nextItem            ();
        ImVec2              getButtonSize       () const { return m_buttonSize; }
        
    private:
        ImGuiAxis           m_toolbarAxis       = ImGuiAxis_X;
        ImVec2              m_buttonSize        = ImVec2(0, 0);
    };
}