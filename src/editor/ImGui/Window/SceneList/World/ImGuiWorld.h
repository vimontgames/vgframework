#pragma once

#include "editor/ImGui/Window/SceneList/ImGuiSceneList.h"

namespace vg::editor
{
    class ImGuiWorld : public ImGuiSceneList
    {
    public:
        ImGuiWorld();

        virtual void                    DrawGUI                 () final override;
    };
}