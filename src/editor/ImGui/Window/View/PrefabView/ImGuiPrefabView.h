#pragma once

#include "editor/ImGui/Window/View/ImGuiView.h"

namespace vg::core
{
    class IBaseScene;
}

namespace vg::editor
{
    class ImGuiPrefabView : public ImGuiView
    {
    public:
        VG_CLASS_DECL(ImGuiPrefabView, ImGuiView);

        ImGuiPrefabView(core::IBaseScene * _prefab);
        ~ImGuiPrefabView();

        gfx::IView::Flags GetViewFlags() const final override;
        core::IWorld * GetWorld() const final override;
        void OnCloseWindow() final override;

    private:
        core::IBaseScene *  m_prefabScene = nullptr;
        core::IWorld *      m_prefabWorld = nullptr;
    };
}