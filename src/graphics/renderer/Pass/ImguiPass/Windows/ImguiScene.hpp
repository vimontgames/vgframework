#include "ImguiScene.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    void ImguiScene::display()
    {
        if (ImGui::Begin("Scenes", &m_isVisible))
        {
            const auto * factory = Kernel::getFactory();
            engine::IEngine * engine = (engine::IEngine *)factory->getSingleton("Engine");

            IUniverse * universe = engine->getCurrentUniverse();
            if (universe)
            {
                for (uint i = 0; i < universe->getSceneCount(); ++i)
                {
                    const IScene * scene = universe->getScene(i);
                    if (nullptr != scene)
                    {
                        if (ImGui::TreeNodeEx(scene->getName().c_str(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen))
                        {
                            ISector * root = scene->getRoot();
                            displaySector(root);
                            ImGui::TreePop();
                        }
                    }
                }
            }
        }
        ImGui::End();
    }

    //--------------------------------------------------------------------------------------
    void ImguiScene::displaySector(ISector * root)
    {
        for (uint j = 0; j < root->getChildSectorCount(); ++j)
        {
            ISector * sector = (ISector*)root->getChildSector(j);
            if (ImGui::TreeNodeEx(sector->getName().c_str(), ImGuiTreeNodeFlags_None))
            {
                displaySector(sector);
                ImGui::TreePop();
            }
        }

        for (uint j = 0; j < root->getEntityCount(); ++j)
        {
            IEntity * entity = (IEntity*)root->getEntity(j);

            auto size = ImGui::CalcTextSize(entity->getName().c_str());
            if (ImGui::Selectable(entity->getName().c_str(), entity == getSelectedObject(), ImGuiSelectableFlags_SelectOnClick, size))
                setSelectedObject(entity);
        }
    }
}