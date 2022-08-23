#include "ImguiScene.h"
#include "imgui/imgui.h"

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
                        IGameObject * root = scene->GetRoot();
                        auto flags = ImGuiTreeNodeFlags_Framed;
                        if (nullptr != root)
                            flags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;
                        else
                            flags |= ImGuiTreeNodeFlags_Leaf;

                        const bool open = ImGui::TreeNodeEx(scene->getName().c_str(), flags);

                        m_sceneMenu.Display((IObject*)scene);

                        if (open)
                        {
                            if (nullptr != root)
                                displayGameObject(root);
                            ImGui::TreePop();
                        }
                    }
                }
            }
        }
        ImGui::End();
    }

    //--------------------------------------------------------------------------------------
    void ImguiScene::displayGameObject(IGameObject * _gameObject)
    {
        const auto children = _gameObject->GetChildren();

        bool open = ImGui::TreeNodeEx(_gameObject->getName().c_str(), children.size() > 0 ? (ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen) : ImGuiTreeNodeFlags_Leaf);
        bool openPopup = false;

        m_gameObjectMenu.Display(_gameObject);

        if (open)
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