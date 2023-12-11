#pragma once
#include "editor/ImGui/Toolbar/ImGuiToolbar.h"

namespace vg::editor
{
    class ImGuiMainToolbar : public ImGuiToolbar
    {
    public:
        ImGuiMainToolbar(const core::string & _name, Flags _flags);

        void        DrawButtons     (Align _align) final override;
        core::uint  GetButtonCount  (Align _align) const final override;
    };
}