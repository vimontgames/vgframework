#include "ImGuiPrefabView.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiPrefabView::ImGuiPrefabView(const core::IResource * _prefabRes) :
        ImGuiView(style::icon::Prefab, "View/Prefabs", "Prefabs", ImGuiWindow::StartVisible, gfx::ViewTarget::Editor),
        m_prefabRes(_prefabRes)
    {
        IFactory * factory = Kernel::getFactory();
        m_prefabWorld = (IWorld *)factory->createObject("World", _prefabRes->GetResourcePath());
        m_prefabWorld->SetObjectFlags(ObjectFlags::Prefab, true);
    }

    //--------------------------------------------------------------------------------------
    ImGuiPrefabView::~ImGuiPrefabView()
    {
        VG_SAFE_RELEASE(m_prefabWorld);
    }

    //--------------------------------------------------------------------------------------
    bool ImGuiPrefabView::ShowToolbar()
    { 
        return false; 
    }

    //--------------------------------------------------------------------------------------
    void ImGuiPrefabView::DrawToolbar()
    {
        //if (ImGui::TooltipButton(fmt::sprintf("%s", editor::style::icon::Save).c_str(), true, true, "Save Prefab", style::button::SizeSmall))
        //{
        //
        //}
    }

    //--------------------------------------------------------------------------------------
    gfx::IView::Flags ImGuiPrefabView::GetViewFlags() const
    {
        return super::GetViewFlags() | gfx::IView::Flags::Prefab;
    }

    //--------------------------------------------------------------------------------------
    core::IWorld * ImGuiPrefabView::GetWorld() const
    {
        return m_prefabWorld;
    }

    //--------------------------------------------------------------------------------------
    // Return 'false' if Prefab does not exist anymore and the View needs to be closed
    //--------------------------------------------------------------------------------------
    bool ImGuiPrefabView::UpdateScene()
    {
        if (!m_prefabWorld)
            return false;

        // Check prefab world still points to a valid scene
        const auto worldRes = Editor::get()->getEngine()->GetWorldResource();
        const uint prefabCount = worldRes->GetSceneResourceCount(core::BaseSceneType::Prefab);
        bool found = false;
        for (uint i = 0; i < prefabCount; ++i)
        {
            const auto * prefabRes = worldRes->GetSceneResource(i, core::BaseSceneType::Prefab);
            if (prefabRes == m_prefabRes)
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            m_prefabWorld->RemoveAllScenes(BaseSceneType::Scene);
            return false;
        }

        auto cur = m_prefabWorld->GetActiveScene(BaseSceneType::Scene);
        auto scene = VG_SAFE_STATIC_CAST(IBaseScene, m_prefabRes->getObject());

        if (cur == nullptr || nullptr == scene)
        {
            m_prefabWorld->RemoveAllScenes(BaseSceneType::Scene);
        }
        
        if (cur != scene)
        {
            m_prefabWorld->AddScene(scene, BaseSceneType::Scene);
            m_prefabWorld->SetActiveScene(scene, BaseSceneType::Scene);
        }

        return true;
    }

    //--------------------------------------------------------------------------------------
    void ImGuiPrefabView::OnCloseWindow()
    {
        // Unselect objects in prefab
        ISelection * selection = Editor::get()->getSelection();
        if (selection)
        {
            core::vector<core::IObject *> selected;
            auto & selObjs = selection->GetSelectedObjects();
            for (uint i = 0; i < selObjs.size(); ++i)
            {
                auto * obj = dynamic_cast<IGameObject*>(selObjs[i]);
                if (obj && obj->GetWorld() == m_prefabWorld)
                {
                }
                else
                {
                    selected.push_back(obj);
                }
            }
            selection->SetSelectedObjects(selected);
        }

        Editor::get()->destroyWindow(this);
        VG_SAFE_RELEASE(m_prefabWorld);
    }
}