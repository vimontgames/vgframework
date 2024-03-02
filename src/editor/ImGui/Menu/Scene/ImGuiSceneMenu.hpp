#include "ImGuiSceneMenu.h"
#include "core/IWorld.h"
#include "core/IBaseScene.h"
#include "core/File/File.h"
#include "engine/IWorldResource.h"
#include "editor/ImGui/Window/ImGuiWindow.h"
#include "ImGuiFileDialog/ImGuiFileDialog.h"
#include "editor/ImGui/Window/SceneList/ImGuiSceneList.h"
#include "editor/ImGui/Window/View/PrefabView/ImGuiPrefabView.h"
#include "engine/IResourceManager.h"

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
        IBaseScene * scene = VG_SAFE_STATIC_CAST(IBaseScene, sceneRes->getObject());
        VG_ASSERT(nullptr != scene);

        const auto * factory = Kernel::getFactory();
        auto * fileDialog = ImGuiFileDialog::Instance();
        auto engine = Editor::get()->getEngine();

        const auto & typeInfo = ImGuiSceneList::getGameObjectTreeTypeInfo(m_sceneType);

        bool openPopup = false;
        bool openFileDialog = false;

        bool save = false;
        bool update = false;

        if (ImGui::BeginPopupContextItem())
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
                m_selected = MenuOption::Save;
                m_popup = fmt::sprintf("Save %s As ...", asString(m_sceneType));

                openFileDialog = true;
            }

            if (m_sceneType == BaseSceneType::Prefab)
            {
                ImGui::Separator();

                if (ImGui::MenuItem("Update", nullptr, false))
                    update = true;

                ImGui::BeginDisabled(!scene->hasFile());
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
                m_selected = MenuOption::Close;
                openPopup = true;
                m_popup = ImGui::getObjectLabel("Remove", _object);
                m_popupObject = scene;
            }
            ImGui::EndPopup();
        }

        if (save)
        {
            string filePath = scene->hasFile() ? scene->getFile() : typeInfo.dataFolder + scene->getName() + typeInfo.fileExt;
            factory->saveToXML(scene, filePath);
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
            IGFD::FileDialogConfig config;
            config.path = io::getRootDirectory() + "/" + typeInfo.dataFolder;
            config.fileName = scene->getName() + typeInfo.fileExt;
            config.countSelectionMax = 1;
            config.flags = ImGuiFileDialogFlags_Modal;

            fileDialog->OpenDialog(m_popup, m_popup, typeInfo.fileExt.c_str(), config);
            ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

            openFileDialog = false;
        }

        
        switch (m_selected)
        {
            case MenuOption::Save:
            if (fileDialog->Display(m_popup.c_str(), ImGuiWindowFlags_NoCollapse, style::dialog::Size))
            {
                if (fileDialog->IsOk())
                {
                    string newFilePath = io::addExtensionIfNotPresent(fileDialog->GetFilePathName(), typeInfo.fileExt.c_str());
                    scene->setName(io::getFileNameWithoutExt(newFilePath));
                    factory->saveToXML(scene, newFilePath);
                    status = Status::Saved;
                }

                ImGuiFileDialog::Instance()->Close();
            }
            break;

            case MenuOption::Close:
            {
                if (m_popupObject == scene && ImGui::BeginPopupModal(m_popup.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
                {
                    ImGui::Text("Are you sure you want to close %s \"%s\"?", asString(m_sceneType).c_str(), scene->getName().c_str());

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