#pragma once

#include "editor/ImGui/Window/SceneList/ImGuiSceneList.h"

namespace vg::editor
{
    class ImGuiPrefab : public ImGuiSceneList
    {
    public:
        ImGuiPrefab();

        core::BaseSceneType GetSceneType    () const final override { return core::BaseSceneType::Prefab; }
        void                DrawGUI         () final override;
    };
}