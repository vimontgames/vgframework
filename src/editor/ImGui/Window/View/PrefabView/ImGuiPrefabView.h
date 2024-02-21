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

        ImGuiPrefabView(const core::IResource * _prefabRes);
        ~ImGuiPrefabView();

        gfx::IView::Flags GetViewFlags() const final override;
        core::IWorld * GetWorld() const final override;
        void OnCloseWindow() final override;
        void UpdateScene()final override;

    private:
        //core::IBaseScene *  m_prefabScene = nullptr;
        const core::IResource *   m_prefabRes = nullptr;
        core::IWorld * m_prefabWorld = nullptr;
    };
}