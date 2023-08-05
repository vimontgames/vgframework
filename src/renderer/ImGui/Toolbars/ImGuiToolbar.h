#pragma once
#include "imgui/imgui_internal.h"
#include "renderer/ImGui/Editors/ImGuiEditor.h"

namespace vg::renderer
{
    class ImGuiToolbar : public ImguiEditor
    {
    public:
        ImGuiToolbar(const core::string & _name, Flags _flags);

        virtual void    update          (double _dt) {};
        virtual void    display         ();

        virtual void    buttons         () = 0;

    protected:
        void            nextItem        ();
        bool            toolbarButton   (const core::string & _label, const core::string & _tooltip, bool _enabled, ImGuiButtonFlags _flags = ImGuiButtonFlags_PressedOnDefault_);

    private:
        ImGuiAxis       m_toolbarAxis   = ImGuiAxis_X;
        ImVec2          m_buttonSize    = ImVec2(0, 0);
    };
}