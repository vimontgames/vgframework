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
    class ImGuiSceneList : public ImGuiWindow
    {
    public:
        enum MenuOption
        {
            None = 0,
            AddScene,
            LoadScene,
        };

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
        ImGuiSceneMenu                  m_sceneMenu;
        ImGuiGameObjectSceneEditorMenu  m_gameObjectMenu;
        core::IGameObject *             m_dragAndDropNodeTarget = nullptr;
        core::IGameObject *             m_dragAndDropInterlineTarget = nullptr;
        ImVec4                          m_originalTextColor;
    };
}