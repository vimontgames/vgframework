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
                        displayGameObject(scene->GetRoot());
                    }
                }
            }
        }
        ImGui::End();
    }

    //--------------------------------------------------------------------------------------
    void ImguiScene::displayGameObject(IGameObject * _gameObject)
    {
        //auto size = ImGui::CalcTextSize(entity->getName().c_str());
        //if (ImGui::Selectable(entity->getName().c_str(), entity == getSelectedObject(), ImGuiSelectableFlags_SelectOnClick, size))
        //    setSelectedObject(entity);

        const auto children = _gameObject->GetChildren();

        if (ImGui::TreeNodeEx(_gameObject->getName().c_str(), children.size() > 0 ? (ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen) : ImGuiTreeNodeFlags_Leaf))
        {
            updateSelection(_gameObject);
            
            for (uint j = 0; j < children.size(); ++j)
            {
                IGameObject * child = children[j];
                displayGameObject(child);
            }
            ImGui::TreePop();
        }        
    }
}