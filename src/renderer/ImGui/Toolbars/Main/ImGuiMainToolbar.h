#pragma once
#include "renderer/ImGui/Toolbars/ImGuiToolbar.h"

namespace vg::renderer
{
    class ImGuiMainToolbar : public ImGuiToolbar
    {
    public:
        ImGuiMainToolbar(const core::string & _name, Flags _flags);

        void buttons() final override;
    };
}