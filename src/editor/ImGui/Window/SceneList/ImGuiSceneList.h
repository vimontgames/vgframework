#pragma once

#include "editor/ImGui/Window/ImGuiWindow.h"
#include "editor/ImGui/Menu/Scene/ImGuiSceneMenu.h"
#include "editor/ImGui/Menu/GameObject/SceneEditor/ImGuiGameObjectSceneEditorMenu.h"

namespace vg::core
{
    enum BaseSceneType : core::u8;
}

namespace vg::editor
{
    vg_enum_class(SceneListMenuOption, core::u8,
        None = 0,
        AddScene,
        LoadScene
    );

    class ImGuiSceneList : public ImGuiWindow
    {
    public:

        struct SceneTypeInfo
        {
            core::string    icon;
            core::string    windowName;
            core::string    newLabel;
            core::string    loadLabel;
            core::string    dataFolder;
            core::string    fileExt;
        };

        ImGuiSceneList(core::BaseSceneType _sceneType, const core::string& _icon, const core::string& _path, const core::string& _name, Flags _flags);
        
        static SceneTypeInfo            getGameObjectTreeTypeInfo   (core::BaseSceneType _sceneType);

    protected:
        void                            display                     (core::BaseSceneType _sceneType);
        void                            displayGameObject           (core::IGameObject* root, core::uint* _count);
       
    protected:
        ImGuiTextFilter                 m_filter;
        ImGuiSceneMenu                  m_sceneMenu;
        ImGuiGameObjectSceneEditorMenu  m_gameObjectMenu;
        core::IGameObject *             m_dragAndDropNodeTarget = nullptr;
        core::IGameObject *             m_dragAndDropInterlineTarget = nullptr;
    };
}