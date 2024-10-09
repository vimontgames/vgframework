#pragma once

#include "editor/ImGui/Window/View/ImGuiView.h"
#include "editor/ImGui/Menu/Scene/ImGuiSceneMenu.h"

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

        gfx::ViewFlags           GetViewFlags        () const final override;

        bool                        ShowToolbar         () const final override;
        void                        DrawToolbar         () final override;

        bool                        ShowTitlebarMenu    () const final override { return true; };
        void                        DrawTitlebarMenu    () final override;
        
        bool                        ShowContextMenu     () const final override { return false;}
        ImGuiMenu::Status           DrawContextMenu     () final override;

        core::IWorld *              GetWorld            () const final override;
        void                        OnCloseWindow       () final override;
        bool                        UpdateScene         () final override;

        VG_INLINE core::IResource * getPrefabRes        () const { return m_prefabRes;}

    private:
        core::IResource *           m_prefabRes         = nullptr;
        core::IWorld *              m_prefabWorld       = nullptr;
    };
}