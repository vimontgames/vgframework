#pragma once

#include "editor/ImGui/Window/SceneList/ImGuiSceneList.h"

namespace vg::editor
{
    class ImGuiPrefab : public ImGuiSceneList
    {
    public:
        ImGuiPrefab();

        void DrawGUI() final override;
    };
}