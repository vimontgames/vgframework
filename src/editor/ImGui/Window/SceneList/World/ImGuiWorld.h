#pragma once

#include "editor/ImGui/Window/SceneList/ImGuiSceneList.h"

namespace vg::editor
{
    class ImGuiWorld : public ImGuiSceneList
    {
    public:
        ImGuiWorld();

        core::BaseSceneType GetSceneType    () const final override { return core::BaseSceneType::Scene; }
        void                DrawGUI         () final override;
    };
}