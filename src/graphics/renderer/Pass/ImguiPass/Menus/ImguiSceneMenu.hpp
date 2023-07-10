#include "ImguiSceneMenu.h"
#include "core/IUniverse.h"
#include "core/IScene.h"
#include "graphics/renderer/Pass/ImguiPass/Windows/ImguiEditor.h"
#include "ImGui-Addons/FileBrowser/ImGuiFileBrowser.h"

using namespace vg::core;

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    ImguiMenu::Status ImguiSceneMenu::Display(core::IObject * _object)
    {
        auto status = Status::None;
        IScene * scene = dynamic_cast<IScene*>(_object);
        VG_ASSERT(nullptr != scene);

        const auto * factory = Kernel::getFactory();
        auto & fileBrowser = ImguiEditor::getFileBrowser();

        bool openPopup = false;

        if (ImGui::BeginPopupContextItem())
        {
            const bool hasPath = scene->hasFile();

            ImGui::PushID("SceneMenu");
            if (ImGui::MenuItem("Save Scene", nullptr, false, hasPath))
            {
                string existingFilePath = scene->getName() + ".scene";
                factory->saveToXML(scene, existingFilePath);
                status = Status::Saved;
            }

            if (ImGui::MenuItem("Save Scene As ..."))
            {
                m_selected = MenuOption::Save;
                //fileBrowser.setPath(io::getCurrentWorkingDirectory() + "/data/Scenes");
                fileBrowser.setFilename(scene->getName() + ".scene");
                m_popup = "Save Scene As ...";
                openPopup = true;
            }
            if (ImGui::MenuItem("Close Scene"))
            {
                m_selected = MenuOption::Close;
                openPopup = true;
                m_popup = "Close Scene";
            }
            ImGui::PopID();
            ImGui::EndPopup();
        }

        if (openPopup)
        {
            ImGui::OpenPopup(m_popup);
            openPopup = false;
        }

        switch (m_selected)
        {
            case MenuOption::Save:
            if (fileBrowser.showFileDialog(m_popup, imgui_addons::ImGuiFileBrowser::DialogMode::SAVE, Editor::LoadSaveDialogSize, ".scene"))
            {
                const string newFilePath = fileBrowser.selected_path;

                size_t lastFolder = newFilePath.find_last_of("/");
                string name = newFilePath.substr(lastFolder+1);

                size_t lastDot = name.find_last_of(".");
                name = name.substr(0, lastDot);

                scene->setName(name);

                factory->saveToXML(scene, newFilePath);
                status = Status::Saved;
            }
            break;
            case MenuOption::Close:
            {
                if (ImGui::BeginPopupModal(m_popup, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
                {
                    ImGui::Text("Are you sure you want to close \"%s\?", scene->getName().c_str());

                    if (ImGui::Button("Yes", Editor::ButtonSize))
                    {
                        IUniverse * universe = (IUniverse*)scene->getParent();
                        if (nullptr != universe)
                            universe->removeScene((IScene*)scene);
                        ImGui::CloseCurrentPopup();
                        status = Status::Removed;
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("No", Editor::ButtonSize))
                        ImGui::CloseCurrentPopup();

                    ImGui::EndPopup();
                }
            }
            break;
        }

        return status;
    }
}