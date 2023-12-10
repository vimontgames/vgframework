#pragma once
#include "editor/ImGui/Toolbar/ImGuiToolbar.h"

namespace vg::editor
{
    class ImGuiMainToolbar : public ImGuiToolbar
    {
    public:
        ImGuiMainToolbar(const core::string & _name, Flags _flags);

        void        DrawButtons     () final override;
        core::uint  GetButtonCount  () const final override;
    };
}