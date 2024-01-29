#pragma once

#include "editor/ImGui/Window/GameObjectTree/ImGuiGameObjectTree.h"

namespace vg::editor
{
    class ImGuiWorld : public ImGuiGameObjectTree
    {
    public:
        ImGuiWorld();

        virtual void                    DrawGUI                 () final override;
    };
}