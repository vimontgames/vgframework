#include "ImGuiPrefabView.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiPrefabView::ImGuiPrefabView(const core::IResource * _prefabRes) :
        ImGuiView(style::icon::Prefab, "View/Prefabs", "Prefabs", ImGuiWindow::StartVisible, gfx::ViewTarget::Editor),
        m_prefabRes(_prefabRes)
    {
        //VG_SAFE_INCREASE_REFCOUNT(_prefab);
        //m_prefabScene = _prefab;

        IFactory * factory = Kernel::getFactory();
        m_prefabWorld = (IWorld *)factory->createObject("World", _prefabRes->GetResourcePath());
        //m_prefabWorld->AddScene(_prefab, BaseSceneType::Scene);
    }

    //--------------------------------------------------------------------------------------
    ImGuiPrefabView::~ImGuiPrefabView()
    {
        //VG_SAFE_RELEASE(m_prefabScene);
        VG_SAFE_RELEASE(m_prefabWorld);
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
    void ImGuiPrefabView::UpdateScene()
    {
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