#pragma once
#include "imgui/imgui_internal.h"
#include "editor/ImGui//Window/ImGuiWindow.h"

namespace vg::editor
{
    class ImGuiToolbar : public ImGuiWindow
    {
    public:
        ImGuiToolbar(const core::string & _name, Flags _flags);

        virtual void    DrawGUI             ();
        virtual void    DrawButtons         () = 0;

        static ImVec2   ComputeButtonSize   ();

    protected:
        void            nextItem            ();
        ImVec2          getButtonSize       () const { return m_buttonSize; }
        
    private:
        ImGuiAxis       m_toolbarAxis       = ImGuiAxis_X;
        ImVec2          m_buttonSize        = ImVec2(0, 0);
    };
}