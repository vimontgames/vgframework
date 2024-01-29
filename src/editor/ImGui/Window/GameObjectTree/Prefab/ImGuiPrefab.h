#pragma once

#include "editor/ImGui/Window/GameObjectTree/ImGuiGameObjectTree.h"

namespace vg::editor
{
    class ImGuiPrefab : public ImGuiGameObjectTree
    {
    public:
        ImGuiPrefab();

        void DrawGUI() final override;
    };
}