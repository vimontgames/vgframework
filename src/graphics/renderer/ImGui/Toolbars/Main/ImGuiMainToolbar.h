#pragma once
#include "graphics/renderer/ImGui/Toolbars/ImGuiToolbar.h"

namespace vg::graphics::renderer
{
    class ImGuiMainToolbar : public ImGuiToolbar
    {
    public:
        ImGuiMainToolbar(const core::string & _name, Flags _flags);

        void buttons() final override;
    };
}