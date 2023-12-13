#include "ImGuiSceneMenu.h"
#include "core/IWorld.h"
#include "core/IScene.h"
#include "core/File/File.h"
#include "engine/IWorldResource.h"
#include "editor/ImGui/Window/ImGuiWindow.h"
#include "ImGui-Addons/FileBrowser/ImGuiFileBrowser.h"

using namespace vg::core;

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiMenu::Status ImGuiSceneMenu::Display(core::IObject * _object)
    {
        auto status = Status::None;
        IScene * scene = dynamic_cast<IScene*>(_object);
        VG_ASSERT(nullptr != scene);

        const auto * factory = Kernel::getFactory();
        auto & fileBrowser = ImGuiWindow::getFileBrowser();

        bool openPopup = false;

        if (ImGui::BeginPopupContextItem())
        {
            ImGui::PushID("SceneMenu");
            if (ImGui::MenuItem("Save", nullptr, false))
            {
                string filePath;

                if (_object->hasFile())
                    filePath = scene->getFile();
                else
                    filePath = "data/scenes/" + scene->getName() + ".scene";

                factory->saveToXML(scene, filePath);
                status = Status::Saved;
            }

            if (ImGui::MenuItem("Save As"))
            {
                m_selected = MenuOption::Save;
                //fileBrowser.setPath(io::getCurrentWorkingDirectory() + "/data/Scenes");
                fileBrowser.setFilename(scene->getName() + ".scene");
                m_popup = "Save Scene As ...";
                openPopup = true;
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Remove Scene"))
            {
                m_selected = MenuOption::Close;
                openPopup = true;
                m_popup = "Remove Scene";
            }
            ImGui::PopID();
            ImGui::EndPopup();
        }

        if (openPopup)
        {
            ImGui::OpenPopup(m_popup);
            openPopup = false;
        }

        string ext = ".scene";

        switch (m_selected)
        {
            case MenuOption::Save:
            if (fileBrowser.showFileDialog(m_popup, imgui_addons::ImGuiFileBrowser::DialogMode::SAVE, style::dialog::Size, ext.c_str()))
            {
                string newFilePath = io::addExtensionIfNotPresent(fileBrowser.selected_path, ".scene");;
                scene->setName(io::getFileNameWithoutExt(newFilePath));
                factory->saveToXML(scene, newFilePath);
                status = Status::Saved;
            }
            break;
            case MenuOption::Close:
            {
                if (ImGui::BeginPopupModal(m_popup, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
                {
                    ImGui::Text("Are you sure you want to close Scene \"%s\?", scene->getName().c_str());

                    if (ImGui::Button("Yes", style::button::SizeMedium))
                    {
                        auto worldRes = ImGuiWindow::getEngine()->GetWorldResource();
                        VG_ASSERT(worldRes);

                        IResource * sceneRes = worldRes->FindSceneResource(scene);
                        VG_ASSERT(sceneRes);

                        worldRes->UnloadSceneResource(sceneRes);

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