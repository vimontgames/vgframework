#include "ImGuiSceneMenu.h"
#include "core/IWorld.h"
#include "core/IBaseScene.h"
#include "core/File/File.h"
#include "core/IResourceManager.h"
#include "engine/IWorldResource.h"
#include "editor/ImGui/Window/ImGuiWindow.h"
#include "editor/ImGui/Window/SceneList/ImGuiSceneList.h"
#include "editor/ImGui/Window/View/PrefabView/ImGuiPrefabView.h"
#include "ImGuiFileDialog/ImGuiFileDialog.h"

using namespace vg::core;

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiSceneMenu::ImGuiSceneMenu(BaseSceneType _sceneType) :
        m_sceneType(_sceneType)
    {

    }

    //--------------------------------------------------------------------------------------
    ImGuiMenu::Status ImGuiSceneMenu::Display(core::IObject * _object)
    {
        auto status = Status::None;
        core::IResource * sceneRes = VG_SAFE_STATIC_CAST(core::IResource, _object);
        IBaseScene * scene = VG_SAFE_STATIC_CAST(IBaseScene, sceneRes->GetObject());
        
        // scene can be null during hot-reload
        if (nullptr == scene)
            return ImGuiMenu::Status::Failure;

        const auto * factory = Kernel::getFactory();
        auto engine = Editor::get()->getEngine();

        const auto & typeInfo = ImGuiSceneList::getGameObjectTreeTypeInfo(m_sceneType);

        bool openPopup = false;
        bool openFileDialog = false, saveFileDialog = false;

        bool save = false;
        bool update = false;

        if (ImGui::BeginPopupContextItem(ImGui::getObjectLabel("ImGuiSceneMenu", _object).c_str()))
        {
            if (m_sceneType == BaseSceneType::Prefab)
            {
                if (ImGui::MenuItem("Edit", nullptr, false))
                    Editor::get()->openPrefabView(sceneRes);

                ImGui::Separator();
            }

            if (ImGui::MenuItem("Save", nullptr, false))
                save = true;

            if (ImGui::MenuItem("Save As"))
            {
                m_selected = SceneMenuOption::Save;
                m_popup = fmt::sprintf("Save %s \"%s\" As ...###Save As ...", asString(m_sceneType), scene->GetName().c_str());

                saveFileDialog = true;
            }

            if (m_sceneType == BaseSceneType::Prefab)
            {
                ImGui::Separator();

                if (ImGui::MenuItem("Update", nullptr, false))
                    update = true;

                ImGui::BeginDisabled(!scene->HasFile());
                if (ImGui::MenuItem("Save & Update", nullptr, false))
                {
                    save = true;
                    update = true;
                }
                ImGui::EndDisabled();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Remove"))
            {
                m_selected = SceneMenuOption::Close;
                openPopup = true;
                m_popup = "Remove";
                m_popupObject = scene;
            }
            ImGui::EndPopup();
        }

        if (save)
        {
            if (engine->SaveScene(sceneRes))
                status = Status::Saved;
        }

        if (update)
        {
            auto rm = engine->GetResourceManager();
            rm->Reimport(sceneRes);
        }

        if (openPopup)
        {
            ImGui::OpenPopup(m_popup.c_str());
            openPopup = false;
        }
        else if (openFileDialog)
        { 
            ImGui::OpenFileDialog(m_popup, typeInfo.fileExt.c_str(), typeInfo.dataFolder);
            openFileDialog = false;
        }
        else if (saveFileDialog)
        {
            ImGui::OpenFileDialog(m_popup, typeInfo.fileExt.c_str(), typeInfo.dataFolder);
            saveFileDialog = false;
        }
        
        switch (m_selected)
        {
            case SceneMenuOption::Save:
            if (ImGui::DisplayFileDialog(m_popup))
            {
                if (ImGui::IsFileDialogOK())
                {
                    string newFilePath = io::addExtensionIfNotPresent(ImGui::GetFileDialogSelectedFile(), typeInfo.fileExt.c_str());
                    scene->SetName(io::getFileNameWithoutExt(newFilePath));
                    factory->SaveToXML(scene, newFilePath);
                    status = Status::Saved;
                }

                ImGui::CloseFileDialog();
            }
            break;

            case SceneMenuOption::Close:
            {
                if (m_popupObject == scene && ImGui::BeginPopupModal(m_popup.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
                {
                    ImGui::Text("Are you sure you want to close %s \"%s\"?", asString(m_sceneType).c_str(), scene->GetName().c_str());

                    if (ImGui::Button("Yes", style::button::SizeMedium))
                    {
                        auto worldRes = ImGuiWindow::getEngine()->GetWorldResource();
                        VG_ASSERT(worldRes);

                        IResource * sceneRes = worldRes->FindSceneResource(scene, m_sceneType);
                        VG_ASSERT(sceneRes);

                        worldRes->RemoveSceneResource(sceneRes, m_sceneType);

                        ImGui::CloseCurrentPopup();
                        status = Status::Removed;
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("No", style::button::SizeMedium))
                        ImGui::CloseCurrentPopup();

                    ImGui::EndPopup();
                }
            }
            break;
        }

        return status;
    }
}