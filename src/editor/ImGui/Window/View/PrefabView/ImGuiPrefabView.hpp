#include "ImGuiPrefabView.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiPrefabView::ImGuiPrefabView(const core::IResource * _prefabRes) :
        ImGuiView(style::icon::Prefab, "View/Prefabs", "Prefabs", ImGuiWindow::StartVisible, gfx::ViewportTarget::Editor),
        m_prefabRes((core::IResource*)_prefabRes)
    {
        IFactory * factory = Kernel::getFactory();
        m_prefabWorld = (IWorld *)factory->createObject("World", _prefabRes->GetResourcePath());
        m_prefabWorld->SetObjectFlags(ObjectFlags::Prefab, true);
        VG_SAFE_INCREASE_REFCOUNT(m_prefabRes);
    }

    //--------------------------------------------------------------------------------------
    ImGuiPrefabView::~ImGuiPrefabView()
    {
        VG_SAFE_RELEASE(m_prefabWorld);
        VG_SAFE_RELEASE(m_prefabRes);
    }

    //--------------------------------------------------------------------------------------
    bool ImGuiPrefabView::ShowToolbar() const
    { 
        return false;
    }

    //--------------------------------------------------------------------------------------
    void ImGuiPrefabView::DrawToolbar()
    {
        const auto * factory = Kernel::getFactory();
        auto engine = Editor::get()->getEngine();

        IBaseScene * scene = nullptr;
        if (m_prefabRes)
            scene = VG_SAFE_STATIC_CAST(IBaseScene, m_prefabRes->GetObject());

        ImGui::BeginDisabled(!scene || !scene->hasFile());
        if (ImGui::TooltipButton(fmt::sprintf("%s", editor::style::icon::Save).c_str(), true, true, "Save Prefab", style::button::SizeSmall))
        {
            if (scene && scene->hasFile())
            {
                const string & filePath = scene->getFile();
                factory->saveToXML(scene, filePath);
            }
        }
        
        ImGui::SameLine();

        if (ImGui::TooltipButton(fmt::sprintf("%s", editor::style::icon::Apply).c_str(), true, true, "Save & Update Prefab", style::button::SizeSmall))
        {
            if (scene && scene->hasFile())
            {
                const string & filePath = scene->getFile();
                factory->saveToXML(scene, filePath);
                auto rm = engine->GetResourceManager();
                rm->Reimport(m_prefabRes);
            }
        }

        ImGui::EndDisabled();
    }

    //--------------------------------------------------------------------------------------
    ImGuiMenu::Status ImGuiPrefabView::DrawContextMenu()
    {
        return ImGuiMenu::Status::None;
    }    

    //--------------------------------------------------------------------------------------
    void ImGuiPrefabView::DrawTitlebarMenu()
    {
        if (nullptr == m_prefabRes || nullptr == m_prefabWorld)
            return;
        
        IBaseScene * scene = VG_SAFE_STATIC_CAST(IBaseScene, m_prefabRes->GetObject());
        
        // scene can be null during hot-reload
        if (nullptr == scene)
            return;
        
        const auto * factory = Kernel::getFactory();
        auto engine = Editor::get()->getEngine();

        bool save = false;
        bool update = false;

        if (ImGui::BeginPopupContextItem("ImGuiPrefabViewTitlebarMenu"))
        {
            ImGui::PushStyle(Style::Bold);
            ImGui::Text(fmt::sprintf("Prefab", style::icon::Prefab).c_str());
            ImGui::PopStyle();
            ImGui::Spacing();

            ImGui::BeginDisabled(!scene->hasFile());
            if (ImGui::MenuItem(fmt::sprintf("%s Save", style::icon::Save).c_str(), "Ctrl-S"))
                save = true;
            ImGui::EndDisabled();

            ImGui::BeginDisabled(!scene->hasFile());
            if (ImGui::MenuItem(fmt::sprintf("%s Save & Update", style::icon::Apply).c_str(), "Ctrl-U"))
            {
                save = true;
                update = true;
            }
            ImGui::EndDisabled();

            ImGui::EndPopup();
        }

        if (!ImGui::IsAnyItemActive())
        {
            const bool isCtrlPressed = ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl);

            if (isCtrlPressed && ImGui::IsKeyPressed(ImGuiKey_S))
            {
                save = true;
            }

            if (isCtrlPressed && ImGui::IsKeyPressed(ImGuiKey_U))
            {
                save = true;
                update = true;
            }
        }

        if (save)
        {
            if (scene->hasFile())
            {
                const string & filePath = scene->getFile();
                factory->saveToXML(scene, filePath);
            }
        }

        if (update)
        {
            auto rm = engine->GetResourceManager();
            rm->Reimport(m_prefabRes);
        }
    }

    //--------------------------------------------------------------------------------------
    gfx::IView::Flags ImGuiPrefabView::GetViewFlags() const
    {
        return super::GetViewFlags() | gfx::IView::Flags::Prefab | gfx::IView::Flags::Render;
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
        auto scene = VG_SAFE_STATIC_CAST(IBaseScene, m_prefabRes->GetObject());

        if (cur == nullptr || nullptr == scene)
            m_prefabWorld->RemoveAllScenes(BaseSceneType::Scene);
        
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
        auto * renderer = Editor::get()->getRenderer();

        // Unselect objects in prefab
        ISelection * selection = Editor::get()->getSelection();
        if (selection)
        {
            core::vector<core::IObject *> selected;
            auto & selObjs = selection->GetSelectedObjects();
            for (uint i = 0; i < selObjs.size(); ++i)
            {
                auto * obj = dynamic_cast<IGameObject*>(selObjs[i]);
                if (nullptr == obj || obj->GetWorld() != m_prefabWorld)
                    selected.push_back(obj);
            }
            selection->SetSelectedObjects(selected);
        }

        if (m_viewport)
        {
            auto & viewIDs = m_viewport->GetViewIDs();
            for (const auto & pair : viewIDs)
            {
                if (auto * view = renderer->GetView(pair.second))
                    view->SetWorld(nullptr);
            }
        }

        Editor::get()->destroyWindow(this);
        m_prefabWorld->RemoveAllScenes(BaseSceneType::Scene);
        VG_SAFE_RELEASE(m_prefabWorld);
        VG_SAFE_RELEASE(m_prefabRes);
    }
}